//librería para poder hacer uso de contiki
#include "contiki.h"

/*librería para habilitar comunicación 
   entre nodos a través del medio físico*/
#include "net/netstack.h"

//permite crear protocolos propios
#include "net/nullnet/nullnet.h"

/*ya que recibimos datos del buffer, 
  es necesario copiarlos a una variable de tipo entero 
  sin signo para poder mostrarlos en pantalla*/
#include <string.h>

//va a permitir el uso de bitácoras dentro de contiki
#include "sys/log.h"

//indicar el nombre del módulo
#define LOG_MODULE "Broadcast Receiver App"

//nivel de bitácora al cual se desea acceder. 
#define LOG_LEVEL LOG_LEVEL_INFO

//definición del intervalo de tiempo para recibir información 
#define WAIT_INTERVAL (8 * CLOCK_SECOND)

//creación del proceso
PROCESS(broadcast_receiver, "Broadcast Received Example");

//se indica a contiki procesos a iniciar de manera automática
AUTOSTART_PROCESSES(&broadcast_receiver);

//se define una función que se va a usar cada que se reciba un paquete
void input_callback(const void *data, 
//void indica que se desconoce el tipo de buffer, data va a contener los datos
uint16_t len,//se le da formato a los datos
const linkaddr_t *source, const linkaddr_t *destination) 
/*estructuras de dirección de capa de enlace 
para proporcionar infor de capa física debido al uso del nullnet*/
{
    //valida que el tamaño del paquete que se recibe sea del mismo tamaño esperado
    //cada que reciba un paquete, lo va a guardar en counter y va a mostrar el origen
    if(len == sizeof(unsigned)) {
        //declaración de contador
        unsigned counter;
        //memcpy copia lo que viene en data del buffer al contador 
        memcpy(&counter, data, sizeof(counter));

        //log info muestra en pantalla la informacion que recien se copio con memcpy
        LOG_INFO("Received %u from", counter);
        //se muestra la información de la capa de enlace
        LOG_INFO_LLADDR(source);
        //se concatena la info
        LOG_INFO_("\n");
    }
}

//inicio del hilo de proceso
PROCESS_THREAD(broadcast_receiver, ev, data)
{
    //declaración del timer periodico
    static struct etimer periodic_timer;
    //declaración de un contador
    static unsigned counter;

    //inicialización de proceso
    PROCESS_BEGIN();

    //se indica el formato que se le va a dar a la info contenida en el contador
    nullnet_buf = (uint8_t *) &counter;
    //se define el tamaño del buffer
    nullnet_len = sizeof(counter);

    //nullnet va a mandar llamar a la función de callback cada que llegue un mensaje
    nullnet_set_input_callback(input_callback);

    etimer_set(&periodic_timer, WAIT_INTERVAL);
    //cilo que va a ejecutar todo lo anterior hasta que expire el temporizador
    while(1){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        //cuando expire el temporizador, va a indicar que el nodo está esperando info en consola
        LOG_INFO("Waiting for data..\n");

        //se reinicia el timer
        etimer_reset(&periodic_timer);
    }
    //se termina el proceso
    PROCESS_END();
}