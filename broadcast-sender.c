//librería para poder hacer uso de contiki
#include "contiki.h"

//librería para habilitar comunicación entre nodos a través del medio físico
#include "net/netstack.h"

/*nullnet es una red sin protocolos, por lo que se puede usar el radio 
sin los mismos o crear los propios en caso de ser necesario*/
#include "net/nullnet/nullnet.h"

/*librería que habilita función memcpy 
(copia lo que hay dentro de una variable, a otro espacio de memoria)*/
#include <string.h>

//va a permitir el uso de bitácoras dentro de contiki
#include "sys/log.h"

//indicar el nombre del módulo
#define LOG_MODULE "Broadcast Sending App"

//nivel de bitácora al cual se desea acceder. 
//la info obtenida servirá como datos de depuración de la app
#define LOG_LEVEL LOG_LEVEL_INFO

//constante que indica cada cuanto tiempo se va a enviar dicha info
//clock_send es en ms, por lo que la info se va a enviar cada 8s
#define SEND_INTERVAL (8 * CLOCK_SECOND)

//creación del proceso
PROCESS(broadcast_sender, "Broadcast Sender Example");
// se indica a contiki los procesos que se van a iniciar de manera automática
AUTOSTART_PROCESSES(&broadcast_sender);

//crear el hilo
//parámetros: proceso, eventos y datos de entrada
PROCESS_THREAD(broadcast_sender, ev, data)
{
    //declaración del temporizador de eventos
    static struct etimer periodic_timer;
    //declaración de contador sin signo
    static unsigned counter = 0;
    
    //inicio de proceso
    PROCESS_BEGIN();

    /* buffer de todo lo que contiene counter, se envía pro medio físico o red 802.15.4
       uint8_t *  es un casting, existe ya que counter tiene tipo de dato indefinido 
       y los datos se requieren en enteros sin signo de 8 bits o un byte. 
       el puntero * significa que es un bufer infinito en lugar de indicar cantidad de datos */
    nullnet_buf = (uint8_t *) &counter;
    /*se determina el tamaño del buffer a traves 
      de la contabilización de lo que contiene counter*/
    nullnet_len = sizeof(counter);

    /*inicializa timer periodico y send interval 
      se refiere a cada cuanto tiempo se va a enviar info (8s)*/
    etimer_set(&periodic_timer, SEND_INTERVAL);
    
    /*todo lo que este dentro del ciclo se va a repetir 
    hasta que el timer periodico expire para evitar que se trabe el programa*/
    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        /*muestra la información del programa siempre y cuando 
          se haya definido el nivel de info a obtener*/
        LOG_INFO("Sending %u\n", counter);

        //guarda lo que hay en counter a buffer y se indica el tamaño del mismo
        memcpy(nullnet_buf, &counter, sizeof(counter));
        nullnet_len = sizeof(counter);

        /* las dos lineas anteriores se repiten ya que es necesario 
           tener la info actualizada 
           porque netstack_network toma los valores automáticamente*/
        NETSTACK_NETWORK.output(NULL);
        /*el incremento es cada que se envíe un paquete 
          para saber cuantos se envían por lapso de tiempo*/
        counter++;

        //se reinicia el temporizador periodico
        etimer_reset(&periodic_timer);
    }
    //fin del proceso
    PROCESS_END();
}