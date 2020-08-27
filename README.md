# UA3REO-DDC-Transceiver
DDC SDR Tranceiver project https://ua3reo.ru/

Проект является учебным, в нём постоянно что-то меняется, дорабатывается. Поэтому собирать только на свой страх и риск, вы можете зря потерять свои деньги и время.<br>

## Принцип работы

ВЧ сигнал оцифровывается высокоскоростной микросхемой АЦП, и подаётся на FPGA процессор.<br>
В нём происходит DDC/DUC преобразование (цифровое смещение частоты вниз или вверх по спектру) - по аналогии с приёмником прямого преобразования.<br>
I и Q квадратурные сигналы, полученные в ходе преобразований, поступают на микропроцессор STM32.<br>
В нём происходит фильтрация, (де)модуляция и вывод звука на аудио-кодек/USB. Также он обрабатывает весь пользовательский интерфейс.<br>
При передаче процесс происходит в обратном порядке, только в конце цепочки стоит ЦАП, преобразующий цифровой сигнал обратно в аналоговый ВЧ.<br>

## Технические характеристики

* Частота приёма: 0.5mHz - 450+mHz
* Частота передачи: 0.5mHz - 30mHz
* Мощность TX: 5Вт+
* Виды модуляции (TX/RX): CW, LSB, USB, AM, FM, WFM, DIGI
* Предусилитель на 20дБ и аттенюатор на 12дБ
* Динамический диапазон АЦП 100дБ
* Напряжение питания: 13.8в
* Потребляемый ток при приёме: ~0.7А
* Потребляемый ток при передаче: ~1.7А+

## Функции трансивера

* Панорама (спектр+водопад) шириной в 96кГц
* Регулируемая полоса пропускания: ФВЧ от 0гц до 500гц, ФНЧ от 300гц до 20кГц
* Автоматический и ручной Notch фильтр
* Отключаемое АРУ (AGC) с регулируемой скоростью атаки
* Карта диапазонов, с возможностью автоматического переключения моды
* Цифровое уменьшение шумов (DNR), подавитель импульсных помех (NB)
* CAT/PTT виртуальные COM-порты (эмуляция FT-450)
* Работа по USB (передача звука, CAT, KEY, PTT)
* CW декодер, самоконтроль
* Анализатор спектра
* TCXO стабилизация частоты (возможно использование внешнего источника 10мГц, например GPS)
* Работа по WiFi: Синхронизация времени, виртуальный CAT-интерфейс (см. Scheme/WIFI-CAT-instruction.txt)

### Чувствительность

При соотношении сигнал-шум 10dB, LNA включен, ATT, LPF, BPF выключены

Частота, mHz | Чувствительность, dBm | Чувствительность
------------ | ------------- | -------------
1.9	| -131	| 63.0 nV
3.6	| -131	| 63.0 nV
7.1	| -131	| 63.0 nV
10  | -131	| 63.0 nV
14  | -131  | 63.0 nV
18  | -131  | 63.0 nV
21  | -131  | 63.0 nV
25  | -131  | 63.0 nV
28  | -131  | 63.0 nV
50	| -131	| 63.0 nV
100	| -131	| 63.0 nV
145	| -128	| 88.9 nV
435	| -121	| 0.2 uV

## Сборка и прошивка

Платы заказывал в китайском сервисе JLCPCB, они и их схемы находятся в папке Scheme.<br>
После сборки необходимо прошить FPGA и STM32 микросхемы.<br>
Прошивка STM32 производится через Keil или по USB шнурку в DFU Mode (скриптом STM32/FLASH.bat). Либо через отладочный шнурок ST-LINK v2. Во время прошивки надо держать зажатой кнопку питания.<br>
Прошивка FPGA происходит через программу Quartus шнурком USB-Blaster.<br>
Правильно собранный аппарат отладки не требует, но при возникновении проблем первым делом надо проверить наличие тактовых сигналов:<br>
90 ножка FGPA и тактовый вход АЦП - 122.88мГц, PC9 ножка STM32 - 12.288мГц, PB10 ножка STM32 - 48кГц.<br>
При необходимости, для калибровки и точной натройки трансивера нужно отредактировать файл settings.h, например можно указать полосы пропускания ДПФ фильтров, инвертировать энкодер и т.д.(всё подписано)<br>
WiFi модуль ESP-01 должен иметь свежую прошивку с SDK 3.0.4 и выше, и AT командами 1.7.4 и новее<br>

## Управление

* **AF GAIN** - Громкость
* **SHIFT/GAIN** - При активной функции SHIFT - плавная отстройка от выбранной частоты трансивера. При неактивной - регулировка усиления ПЧ
* **ENC MAIN** - Главный энкодер для управления частотой и настройками меню
* **ENC 2** - Вспомогательный энкодер для работы с меню. В обычном режиме быстро переключает частоту
* **BAND -** - Переключение на диапазон ниже
* **BAND +** - Переключение на диапазон выше
* **MODE +** - Переключение группы мод SSB->CW->DIGI->FM->AM
* **MODE -** - Переключение подгруппы мод LSB->USB, CW_L->CW_U, DIGI_U->DIGI_L, NFM->WFM, AM->IQ->LOOP
* **FAST** - Режим ускоренной х10 перемотки частоты основным энкодером
* **PRE/ATT** - Переключение режимов предусилителя (МШУ-LNA) и аттенюатора
* **PRE/ATT[зажатие]** - Включение драйвера и/или усилителя АЦП
* **TUNE** - Включение несущей для настройки антенны
* **A/B** - Переключение между банками настроек приёмника VFO-A/VFO-B
* **A/B[зажатие]** - Включение двойного приёмника A&B (в каждом канале наушников свой тракт) и A+B (смешивание сигналов 2-х приёмников)
* **AGC** - Включение АРУ (автоматической регулировки усиления)
* **AGC[зажатие]** - Переключение на меню выбора мощности
* **DNR** - Включение цифрового шумоподавления
* **DNR[зажатие]** - Переключение на меню выбора скорости ключа (WPM)
* **A=B** - Установка настроек второго банка приёмника равным текущему
* **A=B[зажатие]** - Переключение на меню выбора полосы пропускания
* **NOTCH** - Включение автоматического Notch-фильтра для устранения узкополосной помехи
* **NOTCH[зажатие]** - Включение ручного Notch-фильтра для устранения узкополосной помехи
* **CLAR** - Позволяет разнести передачу и приём на разные банки VFO
* **CLAR[зажатие]** - Включение регулировки SHIFT с лицевой панели
* **MENU** - Переход в меню
* **MENU[зажатие]** - Включение блокировки клавиатуры LOCK
* **MENU[при включении]** - Сброс настроек трансивера

## Настройки

### TRX Settings

* **RF Power** - Мощность передачи, %
* **Band Map** - Карта диапазонов, автоматически переключает моду в зависимости от частоты
* **AutoGainer** - Автоматическое управление ATT/PREAMP в зависимости от уровня сигнала на АЦП
* **LPF Filter** - Управление аппаратным ФНЧ Фильтром
* **BPF Filter** - Управление аппаратным полосовым фильтром
* **Two Signal tune** - Двухсигнальный генератор в режиме TUNE (1+2кГц)
* **Shift Interval** - Диапазон расстройки SHIFT (+-)
* **Freq Step** - Шаг перестройки частоты основным энкодером
* **Freq Step FAST** - Шаг перестройки частоты основным энкодером в режиме FAST
* **Freq Step ENC2** - Шаг перестройки частоты основным доп. энкодером
* **Freq Step ENC2 FAST** - Шаг перестройки частоты основным доп. энкодером в режиме FAST
* **DEBUG Console** - Вывод отладочной и служебной информации в USB/UART порты
* **MIC IN** - Выбор микрофонного входа
* **LINE IN** - Выбор линейного входа
* **USB IN** - Выбор входа аудио по USB

### AUDIO Settings

* **IF Gain, dB** - Усиление ПЧ
* **AGC Gain target, dBFS** - Максимальное усиление AGC
* **Mic Gain** - Усиление микрофона
* **Noise Blanker** - Подавитель коротких импульсных помех
* **DNR xxx** - Подстройка цифрового шумоподавителя
* **SSB HPF Pass** - Частота среза ФНЧ при работе в SSB
* **SSB LPF Pass** - Частота среза ФВЧ при работе в SSB
* **CW LPF Pass** - Частота среза ФНЧ при работе в CW
* **FM LPF Pass** - Частота среза ФНЧ при работе в FM
* **FM Squelch** - Уровень шумодава FM
* **MIC EQ xxx** - Уровни эквалайзера микрофона
* **RX EQ xxx** - Уровни эквалайзера приёмника
* **RX AGC Speed** - Скорость срабатывания АРУ (автоматического регулятора уровня сигнала) на приём (больше-быстрее)
* **TX AGC Speed** - Скорость срабатывания АРУ/компрессора на передачу (больше-быстрее)

### CW Settings

* **CW Key timeout** - Время до остановки режима передачи после отпускания ключа
* **CW Generator shift** - Отстройка генератора приёма от частоты передачи
* **CW Self Hear** - Самоконтроль CW (слышно нажатие ключа)
* **CW Keyer** - Автоматический ключ
* **CW Keyer WPM** - Скорость ключа, WPM
* **CW Decoder** - Программный декодер CW приёма

### SCREEN Settings

* **S-METER Marker** - Внешний вид S-Метра (свечка или линия)
* **FFT Zoom** - Приближение спектра FFT (X1 - 96кГц, X2 - 48кГц, X4 - 24кГц, X8 - 12кГц, X16 - 6кГц)
* **FFT Style** - Стиль отображения FFT и водопада
* **FFT Enabled** - Включение водопада и FFT
* **FFT Averaging** - Уровень усреднения всплесков FFT
* **FFT Window** - Выбор окна FFT (Hamming/Blackman-Harris/Hanning)

### ADC/DAC Settings

* **ADC Driver** - Включение предусилителя-драйвера АЦП
* **ADC Preamp** - Включение предусилителя, встроенного в АЦП
* **ADC Dither** - Включение дизеринга АЦП для приёма слабых сигналов
* **ADC Randomizer** - Включение шифрования цифровой линии АЦП
* **ADC Shutdown** - Выключение АЦП

### WIFI Settings

* **WIFI Enabled** - Включение WiFi модуля
* **WIFI Select AP** - Выбор точки доступа WiFi
* **WIFI Set AP Pass** - Установка пароля для точки доступа WiFi
* **WIFI Timezone** - Временная зона (для обновления времени через интернет)
* **WIFI CAT Server** - Сервер для приёма CAT команд по WIFI

### Calibration [появляется при долгом нажатии кнопки MENU в меню настроек]

* **Encoder invert** - Инвертировать вращение основного энкодера
* **Encoder2 invert** - Инвертировать вращение дополнительного энкодера
* **Encoder debounce** - Время устранения дребезга контактов основного энкодера
* **Encoder2 debounce** - Время устранения дребезга контактов дополнительного энкодера
* **Encoder slow rate** - Коэффициент замедления основного энкодера
* **Encoder on falling** - Энкодер срабатывает только на падение уровня A
* **CIC Shift** - Битовое смещение после выхода CIC дециматора
* **CICCOMP Shift** - Битовое смещение после CIC компенсатора
* **TX CICCOMP Shift** - Битовое смещение после TX CIC компенсатора
* **DAC Shift** - Битовое смещение выхода на ЦАП
* **RF GAIN xx** - Калибровка максимальной выходной мощности на каждый диапазон
* **S METER** - Калибровка S-метра
* **ADC OFFSET** - Калибровка смещения АЦП
* **ATT DB** - Параметры аттенюатора
* **LNA GAIN DB** - Параметры LNA
* **LPF END** - Параметры ФНЧ фильтра
* **BPF x** - Параметры полосовых фильтров
* **HPF START** - Параметры ФВЧ фильтра
* **SWR TRANS RATE** - Подстройка коэффициента трансформации SWR-метра
* **VCXO Freq** - Подстройка частоты опорного генератора

### Set Clock Time

* Установка часов

### Flash update

* Запуск обновления прошивки STM32

### Spectrum Analyzer

* **Spectrum START** - Запуск спектрального анализатора
* **Begin, kHz** - Стартовая частота анализатора с шагом в 1kHz
* **End, kHz** - Конечная частота анализатора с шагом в 1kHz
* **Top, dBm** - Верхний порог графика
* **Bottom, dBm** - Нижний порог графика
