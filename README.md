[Monitoreo de Biodigestor en ThingSpeak](https://thingspeak.mathworks.com/channels/3102186)

<h3>Funcionamiento del Sistema de Monitoreo del Biodigestor</h3>

El sistema de monitoreo del biodigestor desarrollado utiliza una **ESP32** como unidad central de procesamiento, encargada de adquirir, procesar y transmitir los datos obtenidos por diversos sensores instalados tanto en el interior como en el entorno del biodigestor. Este sistema permite supervisar variables físico-químicas clave del proceso de digestión anaerobia en tiempo real, facilitando su análisis mediante una plataforma IoT.

En primer lugar, el sensor **DHT22** mide la temperatura y la humedad relativa del ambiente externo. Estos parámetros son importantes para evaluar las condiciones del entorno en el que opera el biodigestor, ya que pueden influir indirectamente en su rendimiento. Las lecturas se realizan mediante una señal digital y se almacenan en variables para su posterior envío.

<img width="197" height="255" alt="image" src="https://github.com/user-attachments/assets/f5e1138b-ca1b-4e7e-b943-44f767c98a74" />

La temperatura interna del biodigestor se mide mediante un **termopar tipo K acoplado a un módulo MAX6675**, el cual convierte la señal analógica del termopar en datos digitales mediante comunicación SPI. Este sensor permite monitorear con precisión la temperatura del proceso biológico, un factor crítico para la actividad de las bacterias anaerobias. El sistema incluye verificación de errores para detectar fallos de conexión o lectura.

<img width="330" height="330" alt="image" src="https://github.com/user-attachments/assets/2f3cab89-a663-47a4-91a1-c90a3dac4b65" />

La humedad interna del sustrato se obtiene mediante el sensor **HW-103**, el cual mide la conductividad del medio y entrega un valor analógico proporcional al contenido de humedad. Este valor es convertido a porcentaje dentro del microcontrolador, permitiendo estimar el nivel de humedad presente en los residuos orgánicos.

<img width="330" height="330" alt="image" src="https://github.com/user-attachments/assets/c13b9100-1529-4590-b53c-070f22c46302" />

La concentración de gases generados en el biodigestor, principalmente metano y otros gases combustibles, se estima mediante el sensor **MQ-5**. Este sensor funciona **midiendo la variación de resistencia interna en presencia de gases**. El código realiza un proceso de calibración inicial para obtener el valor de referencia en aire limpio (Ro), y posteriormente calcula la relación Rs/Ro para estimar la concentración en partes por millón (ppm) mediante ecuaciones logarítmicas basadas en curvas características del sensor.

<img width="225" height="225" alt="image" src="https://github.com/user-attachments/assets/cc9619ab-2d3e-4610-8a3c-5abef9de5e23" />

El nivel de pH del medio se mide utilizando el sensor **PH-4502C**. Para mejorar la precisión de la medición, el sistema realiza múltiples lecturas consecutivas, ordena los valores y promedia los datos intermedios para reducir el ruido. Posteriormente, el valor analógico es convertido a voltaje y transformado en unidades de pH mediante una ecuación lineal calibrada, lo que permite evaluar la acidez o alcalinidad del biodigestor.

<img width="330" height="330" alt="image" src="https://github.com/user-attachments/assets/10c13db5-3a43-4bc4-a190-be297949cfae" />

Una vez que todos los sensores han sido leídos mediante la función updateSensors(), los datos son procesados y almacenados en variables globales dentro de la ESP32. Posteriormente, el microcontrolador utiliza su conectividad WiFi integrada para establecer una conexión con una red inalámbrica disponible.

A continuación, el sistema realiza una petición HTTP al servidor de la plataforma ThingSpeak, enviando cada variable medida como un campo dentro del canal configurado. Esta comunicación se realiza mediante el protocolo HTTP utilizando un cliente TCP, incluyendo parámetros como temperatura ambiente, humedad, temperatura interna, humedad del sustrato, concentración de gas y nivel de pH.

Finalmente, ThingSpeak almacena los datos en la nube, permitiendo su visualización en tiempo real, así como el análisis histórico mediante gráficas. El sistema respeta el intervalo mínimo de actualización de la plataforma, enviando datos aproximadamente cada 20 minutos, lo que garantiza un funcionamiento estable y evita el rechazo de las solicitudes.

En conjunto, este sistema integra sensores ambientales, procesamiento de señales y conectividad IoT para crear una herramienta capaz de monitorear el estado interno de un biodigestor, facilitando la supervisión del proceso biológico y la toma de decisiones basada en datos.

<h3>Desarrollo de un PCB para la Optimización del Sistema</h3>

Para complementar el desarrollo del sistema, se diseñó una placa de circuito impreso (PCB) utilizando la herramienta KiCad, la cual fue posteriormente fabricada mediante el servicio de JLCPCB. Este paso representó una mejora significativa respecto al prototipo inicial implementado sobre protoboard, ya que permitió optimizar la disposición de los componentes, reducir el ruido eléctrico y aumentar la confiabilidad general del sistema.

<img width="716" height="452" alt="image" src="https://github.com/user-attachments/assets/1cc51407-9097-48b5-b93a-a5ff847e0b30" />
[Esquemático del Sistema de Monitoreo](esquematico_biodigestor.pdf)

A diferencia del montaje previo, donde se empleaban numerosos cables y conexiones tipo jumper, la PCB integra todas las rutas eléctricas de manera ordenada y permanente. Esto no solo disminuye la probabilidad de fallos por falsos contactos, sino que también facilita el ensamblaje, mantenimiento y replicabilidad del proyecto. Además, el diseño fue adaptado específicamente a los sensores utilizados, asegurando una correcta distribución de pines y una integración más eficiente con la ESP32 DevKit V1.

<img width="398" height="460" alt="image" src="https://github.com/user-attachments/assets/2006ae65-1bfc-4e15-b16b-1f10609a8f69" />

Otro aspecto relevante es la incorporación de divisores de voltaje implementados con componentes SMD (Surface Mount Device). En el prototipo inicial, estas resistencias estaban soldadas de manera improvisada (“al aire”) y conectadas mediante jumpers, lo que podía introducir errores de medición e inestabilidad. En la PCB, estos divisores están correctamente dimensionados e integrados en el diseño, mejorando la precisión de las lecturas analógicas y la estética del circuito.

<img width="554" height="614" alt="image" src="https://github.com/user-attachments/assets/792e3aa2-69f0-4564-86bd-60b07e7cb6b5" />

Asimismo, la placa incluye un espacio dedicado para la integración de un regulador de voltaje tipo step-up/step-down, capaz de proporcionar una salida estable de 5 V a partir de un rango de entrada entre 2 V y 16 V. Esta característica brinda mayor flexibilidad en la alimentación del sistema, permitiendo el uso de distintas fuentes de energía, como baterías o fuentes externas, sin comprometer el funcionamiento de los sensores ni del microcontrolador.

Finalmente, se añadieron capacitores de desacoplo ubicados estratégicamente cerca de la entrada de alimentación de la ESP32. Estos componentes cumplen la función de filtrar ruidos y estabilizar el voltaje, lo cual es fundamental en sistemas con múltiples sensores analógicos y digitales, ya que contribuye a obtener mediciones más confiables y a proteger los dispositivos ante fluctuaciones en la alimentación.
