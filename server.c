#include "socketP.h"

int main( int argc, char **argv ) {
	int portNumber, serverSocket, clientSocket, clientAddressSize, threadid, type;
	struct hostent *server;
	pthread_t child;
	if ( argc < 3 ) {
		printf( RED"Use %s IP[*.*.*.*] portNumber[1...65535] [S|C]"WHITE"\n", argv[0] );
		exit( -1 );
	}
	//Извлечение порта
	portNumber = atoi( argv[2] );
	if( portNumber > 65535 ) {
		printf( RED"No such port ERROR"WHITE"\n" );
		exit( -1 );
	}
	//Извлечение хоста
    server = gethostbyname( argv[1] );
    if ( server == NULL ) {
        perror( RED"No such host ERROR"WHITE );
        exit( -1 );
    }
    //Определение типа приложения (клиент или сервер)
	type = strcmp( argv[3], "C" );
	//Создание TCP Socket
	serverSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if ( serverSocket < 0 ) {
		perror( RED"Socket creating ERROR"WHITE );
		exit( -1 );
	}
	//Зануление serverAddress
	bzero( (char *) &serverAddress, sizeof( serverAddress ) );
	//Связывание сокета с локальным адресом
	serverAddress.sin_family = AF_INET;
	if( type == 0 ) {
		//Для работы клиента
		//Копирование h_length байтов из server->h_addr в serverAddress.sin_addr.s_addr  
    	bcopy( (char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length );
	} else {
		//Для работы сервера
		//Прием подключений на все IP-адреса
		serverAddress.sin_addr.s_addr = INADDR_ANY;
	}
	//Установка порта
	serverAddress.sin_port = htons( portNumber );
	if ( type == 0 ) {
		//printf( "Client\n" );
		if ( connect( serverSocket, (struct sockaddr *) &serverAddress, sizeof( serverAddress ) ) < 0 ) {
	        perror( RED"Сonnectin ERROR"WHITE );
	        exit ( -1 );
	    }
	    serverFunction( serverSocket );
	} else {
		//printf( "Server\n" );
		//Связывание сокета с адресом
		if ( bind( serverSocket, (struct sockaddr *) &serverAddress, sizeof( serverAddress ) ) < 0 ) {
			perror( RED"Socket binding ERROR"WHITE );
			exit( -1 );
		}
		//Режим ожидания запросов со стороны клиентов
		listen( serverSocket, 1 );
	}
	//Работа сервера
	if ( type != 0 ) {
		while( 1 ) {
			clientAddressSize = sizeof( clientAddress );
			clientSocket = accept( serverSocket, ( struct sockaddr * ) &clientAddress, &clientAddressSize );
			if ( clientSocket < 0 ) {
				perror( RED"Accept ERROR"WHITE );
				exit( -1 );
			}
			serverFunction( clientSocket );
			close( clientSocket );
		}
	}
	return 0;
}

void serverFunction( int clientSocket ) {
	int inputMessage, outputMessage, selectResult;
	char newMessage[1024];
	fd_set socketSelectInput, socketSelectOutput, selectOutput, selectInput;
	struct timeval timeout;
	FD_ZERO( &selectInput );
	FD_ZERO( &selectOutput );
	FD_SET( clientSocket, &selectInput );
	FD_SET( 0, &selectOutput );
	printf( GREEN"Chat started. Enter your name"WHITE"\n" );
	scanf( "%s", &userName );
	strcpy( clientMessage.type, userName );
	strcpy ( clientMessage.data, "Online" ); 
	outputMessage = send( clientSocket, &clientMessage, sizeof( clientMessage ), 0 );
	if ( outputMessage < 0 ) {
		perror( RED"Message send ERROR"WHITE );
		exit( -1 );
	}
	printf( GREEN"You:"WHITE" %s\n", clientMessage.data );
	inputMessage = recv( clientSocket, &clientMessage, sizeof( clientMessage ), 0 );
	if ( inputMessage <= 0 ) {
		perror( RED"Message receive ERROR"WHITE );
		exit( -1 );
	}
	printf( GREEN"%s:"WHITE" %s\n", clientMessage.type, clientMessage.data );
	while( 1 ) {
		printf( RED"\n1"WHITE"\n" );
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		socketSelectOutput = selectOutput;
		selectResult = select( FD_SETSIZE, &socketSelectOutput, (fd_set *)NULL, (fd_set*)NULL, &timeout );
		switch( selectResult ) {
			case 0:
				printf( "No Output\n" );
				break; 
			case -1:
				perror( RED"Select 1 ERROR"WHITE );
				exit( -1 );
			default:
				bzero( &clientMessage, sizeof( clientMessage ) );
				scanf( "%s", &newMessage );
				if ( strcmp( newMessage, "#EXIT" ) == 0 ) {
					close( clientSocket );
				}
				strcpy( clientMessage.data, newMessage );
				strcpy( clientMessage.type, userName );
				send( clientSocket, NULL, sizeof( clientMessage ), 0 );
				outputMessage = send( clientSocket, &clientMessage, sizeof( clientMessage ), 0 );
				if ( outputMessage < 0 ) {
					perror( RED"Message send ERROR"WHITE );
					exit( -1 );
				}
				printf( GREEN"You:"WHITE" %s\n", clientMessage.data );
			    break;
			break;
		}
		printf( RED"2"WHITE"\n" );
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		socketSelectInput = selectInput;
		selectResult = select( FD_SETSIZE, &socketSelectInput, (fd_set *)NULL, (fd_set*)NULL, &timeout );
		switch( selectResult ) {
			case 0:
				printf( "No Input\n" );    
			    break;
			case -1:
				perror( RED"Select 2 ERROR"WHITE );
				exit( -1 );
			default:
				bzero( &clientMessage, sizeof( clientMessage ) );
				inputMessage = recv( clientSocket, &clientMessage, sizeof( clientMessage ), 0 );
				if ( inputMessage < 0 ) {
					perror( RED"Message receive ERROR"WHITE );
					exit( -1 );
				}
				printf( GREEN"%s:"WHITE" %s\n", clientMessage.type, clientMessage.data );
				break;
			break;
		}
	}
}