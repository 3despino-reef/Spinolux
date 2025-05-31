# 🌊 Pantalla LED Modular para Acuario Marino (DIY)

Proyecto de una **pantalla LED casera** para acuarios marinos, completamente modular por canales, económica y controlada con ESP32. Diseñada para reproducir ciclos naturales de luz (amanecer, anochecer, luna) con múltiples canales configurables, sensores de temperatura, protección térmica y conectividad WiFi.


🎥 **[Ver el proyecto en YouTube](https://youtu.be/zrBU30r789M?si=7NmbrzLssdlGKISV)**

---

## ✨ Características principales

- 🔧 **Modular por canales**: hasta 4 canales PWM controlados de forma independiente.
- 💡 **Efectos dinámicos**: simulación de amanecer, anochecer y luz lunar.
- 🌡 **Gestión térmica inteligente**:
  - Sensor de temperatura con alarmas.
  - Control automático de ventilador.
  - Modo de ahorro de energía por sobrecalentamiento.
- 🧠 **Modos de funcionamiento**:
  - `auto`: controla las luces según hora NTP.
  - `cycle`: modo cíclico sin dependencia de red.
  - `on`: encendido manual continuo.
- 🌐 **Conexión WiFi automática** con [WiFiManager](https://github.com/tzapu/WiFiManager).
- ⏱ **Reloj sincronizado por NTP** con ajuste automático de horario de verano.
- 🖥 **Pantalla integrada** con doble vista alternante (estado y configuración).
- 🧪 **Watchdog activado** para máxima estabilidad.

---

## 🧱 Arquitectura del sistema

| Módulo             | Descripción                                                  |
|--------------------|--------------------------------------------------------------|
| `LEDPWMManager`    | Control individual de cada canal LED con transición gradual. |
| `FanManager`       | Activación automática de ventilador por temperatura.         |
| `MenuManager`      | Navegación por menú con botón físico.                        |
| `DisplayManager`   | Pantalla OLED con doble vista rotativa.                      |
| `WiFiManager`      | Configuración automática de red WiFi.                        |
| `NTPClient`        | Sincronización horaria con servidores públicos.              |
| `Preferences`      | Almacenamiento persistente de configuración.                 |

---

## 🧰 Requisitos de Hardware

- ✅ ESP32
- ✅ LEDs de potencia o tiras LED PWM
- ✅ 1 Driver PWM por canal (MOSFET, BJT o módulo)
- ✅ Sensor de temperatura DS18B20 (OneWire)
- ✅ Pantalla OLED (I2C)
- ✅ Botón pulsador (para menú)
- ✅ Ventilador 12V (opcional pero recomendado)
- ✅ Fuente de alimentación 12-24V adecuada

---

## ⚙️ Instalación y uso

1. **Clona el repositorio**:
   ```bash
   git clone https://github.com/3despino-reef/Spinolux
