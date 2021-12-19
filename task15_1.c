//написать программу, которая будет сопротивляться своему завершению
// Для убийства программы в параллельном терминале написать kill -9 <pid>
// ps ux для получения pid процесса

#include <unistd.h>
#include <stdio.h>
#include <signal.h>


volatile int g_last_signal;


//Обрабатываем сигнал
void sig_handler(int signum) {
    g_last_signal = signum;
}


int main(int argc, char *argv[]) {
	if(argc > 1) {
		printf("Too many arguments\n");
		printf("Usage: %s\n", argv[0]);
		return 1;
	}


	//signal устанавливает новый обработчик сигнала с номером в данном случае SIGINT в соответсвие с параметром sig_handler (функция пользователя) SIG_IGN или SIG_DFL.
	//если SIG_IGN то сигнал игнорируется, иначе выполняются стандартны дейтсвия
	//функция signal возвращает предыдущие занчение обработчика сигнала или SIG_ERR при ошибке 
	//Сигналы SIGKILL и SIGSTOP не могут быть пойманы, блокированы или проигнорированы
	if (signal(SIGINT, sig_handler) == SIG_ERR){ //SIGINT (прерывание программы) сигнал посылается, когда пользователь печатает INTR символ (обычно C-c). 
		perror("signal(SIGINT)"); //Функция signal() возвращает SIG_ERR при ошибке  
		return -1;
	} // ^С
	if (signal(SIGQUIT, sig_handler) == SIG_ERR) { //SIGQUIT управляется (символом QUIT), обычно C-\ и производит core-файл, когда он завершает процесс 
		perror("signal(SIGQUIT)");
		return -1;
	} // ^/
	if (signal(SIGTSTP, sig_handler) == SIG_ERR) { //Сигнал SIGTSTP - интерактивный сигнал останова. В отличие от SIGSTOP, этот сигнал может быть обработан и игнорироваться.
		perror("signal(SIGTSTP)");
		return -1;	
	} // ^Z 
	if (signal(SIGHUP, sig_handler) == SIG_ERR) { //SIGHUP ("зависание") сигнал используется, чтобы сообщить, что терминал пользователя разъединен
		perror("signal(SIGHUP)");
        	return -1;
	} 
	if (signal(SIGTERM, sig_handler) == SIG_ERR) { //сигнал, используемый, чтобы вызвать окончание программы. В отличие от SIGKILL, этот сигнал может быть блокирован, обрабатываться, и игнорироваться.
		perror("signal(SIGTERM)");
		return -1;
	} 
    
	while(1) {
		//pause(); -- процесс приостанавливается до тех пор, пока не получит сигнал, хотим заменить на 
		//sigwaitinfo(const sigset_t *set, siginfo_t*info)
		//приостанавливает исполнение вызывающего процесса до тех пор пока не получен один из сигналов из набора set
		const sigset_t set = {SIGINT, SIGQUIT, SIGTSTP, SIGHUP, SIGTERM};
		siginfo_t info;
	        if(sigwaitinfo(&set, &info) == -1) {
			perror("sigwaitinfo");
		}
		printf("\nSignal %d came\n", g_last_signal);
		printf("Number %d\nErrno number %d\nSignal cose %d\nPID %d\nUID %d\n", info.si_signo, info.si_errno, info.si_code, info.si_pid, info.si_uid);
	}
	
	return 0;
}
