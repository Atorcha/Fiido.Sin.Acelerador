/*
                   Version Sin acelerador 1.5.3

                    MODOS DE FUNCIONAMIENTO

   --- MODO 0 - Fija crucero a cualquier velocidad (antiguo) ---

   El arranque progresivo siempre es hasta el tope si no esta fijado
el crucero, si esta fijado hara progresivo hasta le velocidad fijada.

   Una vez en marcha el motor en cualquier momento y siempre que se
esten moviendo los pedales se da un toque de freno y fija la velocidad
y la establece como crucero, aunque se pare la sigue manteniendo hasta
que se desactive.

   Para desactivar el crucero hay que dar dos toques de freno despues
de haber parado los pedales, da igual que este parado o no el motor o
la bici, si se frena antes de pararlos no se desactiva, es muy facil
acostumbrarse a este metodo.

-----------------------------------------------------------------------

               --- MODO 1 - Por Niveles (Nuevo) ---

   En el nuevo modo alternativo (1) la velocidad de crucero se puede ir
aumentando o disminuyendo, con un toque de freno se aumenta y con dos
seguidos se disminuye el nivel o limite de velocidad siempre que se
esten moviendo los pedales.

   En este modo no se pone y quita el crucero como en el anterior
modo 0, siempre habra un nivel seleccionado, para ir a tope poner el
nivel maximo, que debera coincidir con el voltaje maximo. 

    Se pueden configurar la cantidad de niveles que queramos y los
 voltajes/velocidad de cada uno segun los gustos en la lista Niveles,
 hay varios ya configurados y comentados con // por si se quieren usar.

   Que al encender la bici arranque con un modo u otro (0 o 1) se puede
configurar en la variable tipo_crucero, adicionalmente una vez programado
el arduino tambien podemos cambiar de un modo a otro dando 5-6 toques
de freno al encender la bici o tras oir el beep inicial si se ha instalado
el zumbador piezoelectrico.

-----------------------------------------------------------------------

       --- MODO 2 - Sin crucero (velocidad por pedaleo) ---

   En este modo no hay crucero o limite de velocidad, se puede regular
la velocidad a la que queremos ir parando los pedales (dismminuye)
o pedaleando muy despacio (mantiene) la velocidad.

   Este modo se puede acceder a el dando mas de 7 toques de freno
seguidos al encender la bici o tras oir el beep inicial.

--------------------- NUEVO desde la version 1.4 ---------------------

  Incorporado sistema de ayuda al arranque para la salida cuesta arriba
o semaforos, estando parado damos cuatro toques de freno seguidos (maximo 1/3
de segundo entre toques) y hara un progresivo de un corto tiempo que si le
acompanyamos de pedal saldremos sin esfuerzo, en caso de que se activara por
error con otro toque mas de freno se pararia y se anula la ayuda.
   Este sistema esta desactivado por defecto,, para activarlo hay que encender
la bicicleta con cualquiera de los dos manetas de frenos apretada.

--------------------- NUEVO desde la version 1.5 ---------------------

   Nueva funcion auto-progresivo, si se deja de pedalear el motor se para como
de costumbre, pero si continuamos pedaleando antes de transcurridos 10 segundos
no inciara el progresivo desde cero si no que el motor continuara a una velocidad
ligeramente inferior a la que ibamos, si se frena antes de los 10 segundos se
anula la funcion y comenzara progresivo desde cero.

    Ajustes en el algoritmo de aceleracion progresiva (mas suave) y en el modo
de aceleracion por pedaleo.
-----------------------------------------------------------------------
   
   Opcionalmente puede incorporarse un zumbador piezoelectrico en el pin D11
y GND y sonara al activar o desactivar el crucero, subir o bajar nivel,
seleccion de modos, ayuda salida en cuesta y alcanzar el final del progresivo.

 -----------------------------------------------------------------------
 
   El acelerador no haria falta conectarle, se puede desmontar o bloquear,
si gira no hace nada, no se lee el voltaje de el. Este programa esta pensado
explicitamente para ir sin acelerador.

   Las conexiones son las mismas que en anterior montaje de legalizacion del
acelerador excepto la conexion de salida del acelerador al pin A0 que no haria
falta conectarla.

-----------------------------------------------------------------------------

   Desarrollado por ciberus partiendo del programa incial de legalizacion del
acelerador de Fulano, MIL GRACIAS por su ayuda :-) tambien gracias a David
por su ayuda con las innumerables pruebas del programa, Dca por su croquis de
conexiones con los cables de los conectores, Pablo y Fulano por los manuales y
en general a todos los integrantes del grupo de desarrollo que sin su granito
de arena no hubiera sido posible llegar hasta aqui.

-----------------------------------------------------------------------------

   Ayuda, sugerencias, preguntas, etc. en el grupo Fiido telegram:
                     http://t.me/FiidoD1Spain
					
   Grupo telegram de desarrollo privado, si vas a montar el circuito y 
   necesitas ayuda o colaborar pide acceso en el general de arriba.
                 
   Canal con montaje, enlaces, programas, etc. http://t.me/fiidolegal

------------------------------------------------------------------------------
*/

// ================ Variables configurables ===========
// ===  Jugando con estos valores se modifica el comportamiento
// >>> Si no sabes lo que estas tocando mejor que no lo toques. ;-) <<<

// Numero de pulsos por 1/4 de segundo para que se considere 
// que se esta pedaleando, configurar segun sensor y gustos
int cadencia = 2;

// Retardo en segundos para parar el motor una vez se deja de pedalear
// Usar multiplos de 0.25
float retardo_paro_motor = 0.50;

// Retardo en segundos para ponerse a velocidad maxima o crucero.  
int retardo_aceleracion = 5;

// Retardo para inciar progresivo desde cero tras parar pedales (freno anula el tiempo)
// Poner a cero para desconectar esta opcion y que inicie siempre desde cero
int retardo_inicio_progresivo = 10;

// Direccion del bus I2C [DAC] (0x60) si esta soldado, si no (0x62)
const int dir_dac = 0x60; 

// Nivel al que se desea iniciar el progresivo
// Aumentar si se desea salir mas rapido, >> NO PASAR DE 2.00 <<                                                                          
const float nivel_inicial_progresivo = 1.5;

// Desacelera al parar los pedales, Poner en true para activar
// o false para que no desacelere. Recomendable TRUE siempre.
boolean desacelera_al_parar_pedal = true;

// Modo crucero true = activado, false no se activa nunca.
boolean modo_crucero = true;

// Tipo por defecto de funcionamiento: 0 = Un toque de freno fija crucero (antiguo),
// 1 = Crucero (nuevo por niveles) sube con un toque y baja con 2 toques,
// y 2 = Velocidad por movimiento de pedales [No tiene crucero]), tambien se pueden
// elegir o cambiar con 5-6 toques o mas de freno al encender la bici.
int tipo_crucero = 1; // Valores = 0, 1 y 2 ...

// Configuracion de los pitidos del zumbador piezoelectrico
const boolean tono_inicial = true;          // Al encender la bici y confirmacion de modos de funcionamiento
const boolean tono_crucero = true;          // Al fijar y des-fijar crucero o cambiar nivel en cualquier tipo
const boolean tono_fin_progresivo = false;  // Al alcanzar el final del progresivo
const boolean tono_arranque = true;         // En el segundo tercer y cuarto toque de freno para activar la salida en cuesta

// Voltajes para los distintos niveles de asistencia en tipo 1 (se pueden anyadir, editar o quitar niveles)
// Hay varios ejemplos comentados por si quieres tener mas niveles descomenta uno y comenta el activo -> (//)
//const float niveles[] = {2.20 , 2.40 , 2.60 , 2.80 , 3.0 , 3.20 , 3.40 , 3.60 , 3.90}; //saltos de 0.20 9n
//const float niveles[] = {2.20 , 2.45 , 2.70 , 2.95 , 3.20 , 3.45 , 3.90}; //saltos de 0.25 7n
//                          8      11     14     17     20     23     26   kmh
//const float niveles[] = {2.15 , 2.45 , 2.75 , 3.05 , 3.35 , 3.90}; // saltos de 0.30 6n
//                          7      12     15     18     22     25-26    kmh
//const float niveles[] = {2.15 , 2.46 , 2.77 , 3.08 , 3.39 , 3.90}; // saltos de .31 6n
//                           7     12     16     20     23     26    kmh
//const float niveles[]= {2.15 , 2.30 , 2.65 , 3.0 , 3.35 , 3.90};   // saltos de +- .35  6n
//                    1.95=nada    8 ,   14     18     22     26    kmh
const float niveles[] = {2.40 , 2.80 , 3.20 , 3.90}; // saltos de 0.40 4n
//                        10     15     20     26      kmh

// Las velocidades medidas son aproximadas, depende de la carga, cuesta, peso y mas factores.
// Aprox 0.20 son saltos de 2,5kmh , 0.25 de 3kmh, 0.35 de 4kmh ,0.40 de 5kmh

// Nivel inicial al encenderr la bici (el primero de la lista de niveles es cero)
int nivel = 3;

// Ayuda al arranue, desactivado por defecto por seguridad por si nos paran,
// ya que es ilegal que el motor funcione sin pedalear, para activarlo hay que
// encender la bici con el freno pulsado.
boolean ayuda_salida_cuesta = false;

//================ FIN VARIABLES CONFIGURABLES POR EL USUARIO ====================

#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;

//======= CONFIGURACION DE LOS PINES ==========
const int pin_pedal = 2; // Pin sensor pas, en nano o uno usar solo 2 o 3
const int pin_freno = 3; // Pin de activacion del freno
const int pin_piezo = 11; // Pin donde se ha conectado el zumbador (9,10,11)

//======= Variables para calculos
// Voltaje maximo a tope, >>> (No pasar de 4.20) <<<
const float voltaje_maximo = 3.90;

// Valor minimo de voltaje (acelerador en reposo)
const float voltaje_minimo = 0.85;

// Tiempo en milisegundos para contar pulsos
const int tiempo_cadencia = 250;

// Contadores de paro, aceleracion y auto_progresivo del motor
unsigned contador_retardo_paro_motor = 0;
int contador_retardo_aceleracion = 0;
unsigned contador_retardo_inicio_progresivo = 0;
int bkp_contador_retardo_aceleracion = 0;
boolean auto_progresivo = false;

// Muestra en el plotter si el motor esta encendido(-5) o no (-8)
int motor = -8;

// Variable donde se almacena la velocidad de crucero
float v_crucero = voltaje_maximo; 

// Los voltios que se mandan a la controladora
float nivel_aceleracion = voltaje_minimo;

// Variable donde se guarda el estado del crucero en modo 0
boolean fijado = false; 

// Contador de pulsos del pedal
int pulsos = 0; 

// Contador de pulsaciones de freno
int frenadas = 0; 

// Switch de aviso de final de progresivo
boolean aviso = false; 

// Modo depuracion (grafica y monitor)
const boolean debug = false;

// Calculo del numero de niveles de asistencia -1
const int num_niveles = (sizeof(niveles)/sizeof(niveles[0]))-1;

//======= Variables interrupcion
volatile int p_pulsos = 0; // Variable donde se suman los pulsos del sensor PAS
volatile int p_frenadas = 0; // Variable donde se suman las pulsaciones del freno

//================== FUNCIONES ==================
void establece_voltaje(){
  float incremento = ((v_crucero+0.3) - nivel_inicial_progresivo) / retardo_aceleracion;
  nivel_aceleracion = nivel_inicial_progresivo + (incremento * contador_retardo_aceleracion);
  if (nivel_aceleracion == nivel_inicial_progresivo){nivel_aceleracion = voltaje_minimo;}
  if (nivel_aceleracion > v_crucero){nivel_aceleracion = v_crucero;}
  uint32_t valor = (4096 / 5) * nivel_aceleracion;
  dac.setVoltage(valor,false); // fija voltaje en DAC                               
}

void pedal(){
   p_pulsos++;
}

void freno(){
  p_frenadas++;
  contador_retardo_inicio_progresivo = retardo_inicio_progresivo;
  bkp_contador_retardo_aceleracion = 0;
  cadencia=2;
}

void para_motor(){
  motor = -8;
  if (contador_retardo_aceleracion > 4){
    bkp_contador_retardo_aceleracion = contador_retardo_aceleracion/1.2;
    cadencia=3;
  }
  contador_retardo_aceleracion = 0;
  aviso = false;
  auto_progresivo = true;
}

void fija_crucero(){
  v_crucero = nivel_aceleracion;
  fijado = true;
  if (tono_crucero) {tone( pin_piezo, 2100, 200);} //Tono activa crucero
  delay(300);
}

void quita_crucero(){
  delay(100);
  p_frenadas = 0;
  delay(350);
  if (p_frenadas > 0) {
    v_crucero = voltaje_maximo;
    fijado = false;
    if (tono_crucero) {tone( pin_piezo, 1900, 150);} //Primer tono (desactiva crucero)
    delay(200);
    if (tono_crucero) {tone( pin_piezo, 1900, 150);} //Segundo tono (desactiva crucero)
    p_frenadas = 0;
  }  
  frenadas = 0;
}

void cambia_nivel(){
  if (tono_crucero) {tone( pin_piezo, 2000, 60);}  //Primer tono (sube de nivel)
  nivel++;
  delay(100);
  p_frenadas = 0;
  delay(350);
  if (p_frenadas > 0) {
    if (tono_crucero) {tone( pin_piezo, 1800, 60);}  //Segundo tono (baja de nivel)
    nivel = nivel - 2;
  }  
  // Comprueba si hemos llegado al nivel maximo
  if (nivel >= num_niveles) {
    if (tono_crucero) {tone( pin_piezo, 2100, 300);}  //Tono largo nivel maximo
    nivel = num_niveles;
  }
  // Comprueba si hemos llegado al nivel minimo
  if (nivel <= 0) {
    delay(90);
    if (tono_crucero) {tone( pin_piezo, 2100, 300);}  //Tono largo nivel nimimo
    nivel = 0;
  }
  v_crucero = niveles[nivel];
  delay(200);
  p_frenadas = 0;
}

void salida_cuesta_arriba(int tiempo){
  if (v_crucero < 3) {tiempo = tiempo + 2;}  // Aumentamos a 2 seg si hay nivel bajo
  p_frenadas = 0;
  for (int ac = 0; ac < tiempo; ac++){
    if (p_frenadas > 0){
      contador_retardo_aceleracion = 0;
      establece_voltaje();
      if (tono_arranque){tone( pin_piezo, 2800, 300);}  //Cuarto tono, anulacion
      break;
    }
    contador_retardo_aceleracion++;
    establece_voltaje();
    //if (debug) {impresion_plotter();}
    delay(tiempo_cadencia);
  } 
}

void ayuda_arranque(){
  p_frenadas = 0;
  delay(350);
  if (p_frenadas > 0) {
    if (tono_arranque){tone( pin_piezo, 2300, 50);}  //Primer tono, segunda frenada
    p_frenadas = 0;
    delay(350);
    if (p_frenadas > 0) {
      if (tono_arranque){tone( pin_piezo, 2500, 50);}  //Segundo tono, tercera frenada
      p_frenadas = 0;
      delay(350);
      if (p_frenadas > 0) {
        if (tono_arranque){tone( pin_piezo, 2700, 50);}  //Tercer tono, cuarta frenada
        p_frenadas = 0;
        delay(75);
        salida_cuesta_arriba(6); // tiempo en cuartos de segundo 6 = 1,5 seg.
      }
    }
  }
}

void impresion_plotter() {
   //===== Impresion plotter serial para grafica
   Serial.print("Pul: ");
   Serial.print(pulsos);
   Serial.print("/");
   Serial.print(cadencia);
   Serial.print(" ,Mot: ");
   Serial.print(motor);
   Serial.print(" ,Fren: ");
   Serial.print(frenadas);
   //Serial.print(" ,Fix: ");   
   //Serial.print(fijado);
   Serial.print(" ,V cr: ");
   Serial.print(v_crucero);
   Serial.print(" ,Niv ac: ");
   Serial.print(nivel_aceleracion);
   Serial.print(" ,Cnt ac: ");
   Serial.print(contador_retardo_aceleracion);
   Serial.print("/");
   Serial.print(bkp_contador_retardo_aceleracion);
   Serial.print(" ,cnt paro: ");
   Serial.print(contador_retardo_paro_motor);
   Serial.print(" ,cnt prog: ");
   Serial.println(contador_retardo_inicio_progresivo);

   //=============================================
}


void setup() {

  if (debug) {
    // Inicia serial:
    Serial.begin(19200);
  
    // Configura plotter
    Serial.println("8,-8,0,0,0,0");
  }

  // Configura pines y prepara las interrupciones
  pinMode(pin_piezo, OUTPUT);
  pinMode(pin_freno, OUTPUT);
  digitalWrite(pin_freno, HIGH);
  pinMode(pin_pedal, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(pin_pedal),pedal,CHANGE); //interrupcion pedal
  attachInterrupt(digitalPinToInterrupt(pin_freno),freno,FALLING); //interrupcion freno
  
  // Configura DAC
  dac.begin(dir_dac);
  dac.setVoltage(810, true); // fija voltaje inicial en Dac (0.85v)

  // Si encendemos la bici con el freno pulsado activamos la ayuda salida en cuesta 
  // Una vez oido el beep inicial ya se puede soltar el freno o dar toques para cambiar de modo
  if (digitalRead(pin_freno) == LOW) {ayuda_salida_cuesta = true;}

  // Selector de modos en el encendido, hay 3 modos configurables (por defecto, modo 1 y 2) dependiendo
  // de las frenadas QUE DEN EN 3 SEGUNDOS, aviso da las que le da la gana no las que damos realmente,
  // emitira dos pitidos para el modo 1 (tipo alternativo de crucero), y 3 para el 2 (velocidad por pedaleo),
  // si no suena nada despues de los 3 seg. del pitido inicial es que se ha inciciado con el modo por defecto
  // configurado en la variable tipo_crucero.
  if (tono_inicial) {tone( pin_piezo, 2000, 50);} //Tono aviso a la espera de frenadas (al encender bici)
  delay(60);
  if (tono_inicial) {noTone(pin_piezo);}
  p_frenadas = 0;
  p_pulsos = 0;
  delay(3000);  //3 segundos para dar las frenadas
  // Modo alternativo de crucero (el contrario al que tengamos configurado por defecto)
  // Activar con 5-6 toques de freno tras oir el pitido inicial o 1 seg despues de girar
  // la llave para encender la bici.
  if (p_frenadas > 5 && p_frenadas < 15){
    if (tono_inicial) {tone( pin_piezo, 1900, 100);}  //Tono 1 verificacion cambio de modo
    delay(150);
    if (tono_inicial) {tone( pin_piezo, 1900, 100);}  //Tono 2 verificacion cambio de modo
    delay(150);
    if (tipo_crucero == 0){
      tipo_crucero = 1;
    }
    else if (tipo_crucero == 1){
      tipo_crucero = 0;
    }
    ayuda_salida_cuesta = true;
    p_frenadas = 0;
  }
  // Modo alternativo al pedalear aumenta velocidad y al parar pedales disminuye
  // Activar con mas de 7 toques de freno tras oir el pitido o encender la bici.
  else if (p_frenadas > 14){
    if (tono_inicial) {tone( pin_piezo, 1900, 100);}  //Tono 1 verificacion cambio de modo
    delay(150);
    if (tono_inicial) {tone( pin_piezo, 1900, 100);}  //Tono 2 verificacion cambio de modo
    delay(150);
    if (tono_inicial) {tone( pin_piezo, 1900, 100);}  //Tono 3 verificacion cambio de modo
    delay(150);
    retardo_aceleracion = 6;
    retardo_paro_motor = 1.50;
    desacelera_al_parar_pedal = true;
    modo_crucero = false;
    tipo_crucero = 2;
    ayuda_salida_cuesta = true;
    p_frenadas = 0;
  }
  
  // Fijamos el nivel seleccionado al encender la bici si aranca en modo 1
  if (tipo_crucero == 1) {v_crucero = niveles[nivel];}

  // Ajusta contadores de tiempo de la configuracion 
  retardo_paro_motor = retardo_paro_motor * (1000 / tiempo_cadencia);
  retardo_aceleracion = retardo_aceleracion * (1000 / tiempo_cadencia);
  retardo_inicio_progresivo = retardo_inicio_progresivo * (1000 / tiempo_cadencia);
  
  // Anulamos el retardo por seguridad para que empiece progresivo al encender la bici
  contador_retardo_inicio_progresivo = retardo_inicio_progresivo;
}

// Bucle principal
void loop() {
  p_pulsos = 0;
  p_frenadas = 0;
  delay(tiempo_cadencia);
  pulsos = p_pulsos;
  frenadas = p_frenadas;
  
  // Si se pedalea despacio o se paran los pedales
  if (pulsos < cadencia) {
      contador_retardo_inicio_progresivo++;
      contador_retardo_paro_motor++;
      if (contador_retardo_paro_motor >= retardo_paro_motor){para_motor();}
  }

  // Si se pedalea normal (por encima de la cadencia).
  if (pulsos >= cadencia) {
      if (contador_retardo_inicio_progresivo < retardo_inicio_progresivo && auto_progresivo){
        contador_retardo_aceleracion = bkp_contador_retardo_aceleracion;
        auto_progresivo = false;
        cadencia=2;
      }
      else { auto_progresivo = false;  }
      contador_retardo_inicio_progresivo = 0;
      contador_retardo_paro_motor = 0;
      if (contador_retardo_aceleracion < retardo_aceleracion){
        contador_retardo_aceleracion++;
      }     
      motor = -5;
  }

  // Si estan los pedales parados
  if (pulsos == 0){
    // Desacelera al parar los pedales
    if (contador_retardo_aceleracion > 0 && desacelera_al_parar_pedal){
      contador_retardo_aceleracion = contador_retardo_aceleracion - 2;
      if (contador_retardo_aceleracion < 0){contador_retardo_aceleracion = 0;}
    }
    // Desactiva crucero solo al pulsar el freno con los pedales parados en modo 0
    if (frenadas > 0 && modo_crucero && fijado){quita_crucero();}
  }

  // Si se ha pulsado el freno
  if (frenadas > 0) {
    // Activa el crucero solo con pedales en movimiento y algunas condiciones (Solo tipo 0)
    if (tipo_crucero == 0 && pulsos > 3 && nivel_aceleracion > 2.10
    && nivel_aceleracion < 3.60 && modo_crucero && !fijado){fija_crucero();}
    
    // Nuevo modo [Tipo 1] sube nivel de asistencia con un toque y baja con 2 toques seguidos
    else if ( modo_crucero && tipo_crucero == 1 && pulsos > 3
    && contador_retardo_aceleracion != 0){cambia_nivel();}

    // Salidas en cuesta 4 toques de freno consecutivos si estamos parados (el primero no suena)
    else if (nivel_aceleracion == voltaje_minimo && contador_retardo_aceleracion == 0
    && contador_retardo_paro_motor >= retardo_paro_motor && ayuda_salida_cuesta){ayuda_arranque();}

    // Para motor al frenar excepto al fijar el crucero en modo 0 (modo 1 nunca lo para)
    else {para_motor();}
  }

  // Aviso de final de progresivo, configurar arriba si no se desea
  if (tono_fin_progresivo){
    if (contador_retardo_aceleracion == retardo_aceleracion && !aviso) {
      tone(pin_piezo, 2900, 500); //Tono de Final de progresivo
      aviso = true;
    }
  }
  
  establece_voltaje();
  if (debug) {impresion_plotter();}
}

// EOF 10/05/2019
