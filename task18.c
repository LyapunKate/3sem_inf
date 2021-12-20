#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <math.h>


typedef struct {
	double (*function)(double);
    
	double start, end;

	unsigned long points;
	double* p_total_sum;

} integrate_info;


// В этой функции считаем интеграл, используя метод трапеций
ssize_t integrate(integrate_info* integral)
{
	//если пределы интегрирования совпадают, то интеграл равен 0
	if(integral->end == integral->start)
		return 0;

	double local_sum = 0;
	double dx = (integral->end - integral->start) / (double)integral->points;

	unsigned long i = 0;
    	while(integral->start + (double)i * dx < integral->end)
    	{
		local_sum += (integral->function(integral->start + (double)i * dx) + integral->function(integral->start + (double)(i + 1) * dx)) / 2.0;

		i++;
	}
	
	if(isnan(local_sum))
	{
		printf("The function is discontinuous at [%lf : %lf]\n", integral->start, integral->end);
		return -1;
	}

	local_sum *= dx;


	*(integral->p_total_sum) += local_sum;

	return 0;
}


int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		printf("Usage: %s <'function(x)'> <start> <end> <#points>\n", argv[0]);
		return 1;
	}

	//создаёт пару файловых описателей и помещает их в массив
	int pipe_fds[2] = {};

	if(pipe(pipe_fds) < 0)
	{
		perror("pipe");
		return 2;
	}

	//порождаем дочерний процесс
	pid_t child_id = fork();
	if(child_id < 0)
	{ 
		perror("fork");
		return 3;
	}

	if(child_id == 0)
	{
		//это происходит в дочернем процессе
		close(pipe_fds[1]);
		//dup2 создаёт дубликать файлового дескриптора
		//например, с помощью такой программы можно перенаправить вывод в файл, на который ссылается pfd
		//int pfd;
		//close(1);
		//dup(pfd);
		//close(pfd);
		//вызов dup2(pipe_fds[0], fileno(stdin)) эквивалентен следующему
		//close(stdin);
		//fcntl(pipe_fds[0], F_DUPFD, fileno(stdin))
		//то есть закрыл ввод через stdin и вводим из pipe_fds[0]
		if(dup2(pipe_fds[0], fileno(stdin)) < 0)
		{
			perror("dup2");
			return 4;
		}
		close(pipe_fds[0]);
		//после ввода его надо закрыть

		//семейство функций exec заменяет текущий образ процесса новым образом процесса
		//первый параметр указывает на имя, ассоциированное с файлом, который надо исполнять
		//остальные параметры представляют собой список параметров, доступных исполняемой программе
		//это указатели на строки, оканчивающиеся на нал
		// Start compilation
		execlp(
                // executable
                "gcc", 
                // argv[0], argv[1], ...
                "gcc", "-Wall", "-Wextra", 
                // produce position-independent dynamic library (shared option)
                "-fPIC", "-fPIE", "-shared",
                // enable optimisations
                "-O2", "-fomit-frame-pointer", "-march=native", "-mtune=native",
                // read input from stdin, treat it as a C source
                "-xc", "-",
                // link with linkm
                "-lm", 
                // save output to tmp.so, потом мы откроем её с помощию dlopen
                "-o", "tmp.so", NULL);

		perror("exec");
		return 5;
    }
    else //если child_id != 0, то это родительский процесс
    {
        //выводим данные в файловый дескриптор
        dprintf(pipe_fds[1],
                "#include <math.h>\n"
                "double tmpfun(double x) { return %s; }\n", argv[1]);
        
        close(pipe_fds[1]);


        //ждём, что при компиляции не было ошибок, проверяем статус
        int status;
        waitpid(child_id, &status, 0);

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) 
        {
            printf("Compilation error, gcc exit code %d\n", WEXITSTATUS(status));
            return 6;
        }

        // подгружаем библиотеку
	// dlopen загружает динамическую библиотеку, имя которой указано первым аргументом, в данном случае ./tmp.so
	// RTLD_LAZY -- флаг подразумевает разрешение в виде кода, содержащегося в исполняемой динамической библиотеке
	// (при другом флаге (RTLD_NOW) требуется разрешение всех неопределённых символов перед возвратом из dlopen)
	// в man есть пример для math.h, здесь сделали аналогично
	// компилировать с -ldl 
        void* tmplib = dlopen("./tmp.so", RTLD_LAZY);
        char* dlerrstr;
        if (!tmplib) {
            perror(dlerror());
            return -1;
        }

        double(*tmpfun)(double);
        *(void**)(&tmpfun) = dlsym(tmplib, "tmpfun");
        if ((dlerrstr = dlerror()) != NULL) {
            puts(dlerrstr);
            return -1;
        }

        double total_sum = 0;

        integrate_info integr = {
            .function = tmpfun,
            .start    = atof(argv[2]),
            .end      = atof(argv[3]),
            .points   = strtoul(argv[4], NULL, 10),
            .p_total_sum = &total_sum
        };

        integrate(&integr);

        printf("Integral of %s (from x=%s to x=%s) = %lf\n", argv[1], argv[2], argv[3], total_sum);
	//закрываем
        dlclose(tmplib);

    }
    return 0;
}
