#include <pthread.h>
#include <fcntl.h> 
#include <unistd.h>

void *read_txt();
void *write_txt();

const char START = 2; /* ASCII STX: Start of Text */
const char END = 3; /* ASCII ETX: End of Text */
const char ACK = 6; /* ASCII ACK: Acknowledge */

char g_byte = START; /* a global variable accessed by all threads */ 

int main() {

     pthread_t thr_read;
     pthread_t thr_write;

     int thread_return;

     /* create thread for reading and exit if error */
     thread_return = pthread_create(&thr_read, NULL, read_txt, NULL);
     if (thread_return != 0) 
          return(thread_return);
     
     /* create thread for writing and exit if error */
     thread_return = pthread_create(&thr_write, NULL, write_txt, NULL);
     if (thread_return != 0) 
          return(thread_return);

     /* Join each newly-created thread to the current thread */
     pthread_join(thr_read, NULL);
     pthread_join(thr_write, NULL);

     return (0);

}

/* Read from the source file one byte at a time */
void *read_txt() {

   int fd;
   int bytes_read;

   fd = open("./hw3.bw.xpm", O_RDONLY);  

   while (1) { 
       if (g_byte == ACK) {
            bytes_read = read(fd, &g_byte, 1);        
            if (!bytes_read) {
                 g_byte = END; 
                 break;       
            }
       } 
   }

   close(fd);
   pthread_exit(NULL); /* A clean way to terminate a thread */
}

/* Invert the image */
void *write_txt() { 

  int fd;
  char black = ' ';
  char dot = '.';

  fd = open("./hw3_inverted.xpm", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

  while(1) {
      if (g_byte != ACK) {
           if (g_byte == END)
               break;
           else {
              if (g_byte == black)
                  write(fd, &dot, 1); 
              else if (g_byte == dot)
                  write(fd, &black, 1); 
              else
                  write(fd, &g_byte, 1); 
                  
              g_byte = ACK;
           }
      }
  }

  fsync(fd);
  close(fd);
  pthread_exit(NULL);

}
