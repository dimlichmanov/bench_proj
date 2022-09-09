###### Требования к сборке:

- python3 с библиотекой plotly 
- PAPI

Сборка пользовательских бенчмарков осуществляется из директории benchmarks_new

###### Пример сборки и запуска с помощью скрипта run_tests.py:

python3 ./run_tests.py --bench=stencil_2D --force="--length=80 --radius=3 --mode=0" --threads-num=3 --compiler=icpc --profile --sockets=1

--bench=stencil_2D - данный параметр позволяет указать название целевого бенчмарка. Список доступных бенчмарков - названия поддиректорий в директории benchmarks_new

При указании только этого параметра, запуск бенчмарка производится с параметрами, указанными в шапке скрипта run_tests.py и заданных в виде соответствующего элемента 
Python-словаря. 

Пример:

"stencil_1D": {"mode": {"min": 0, "max": 1, "step": 1},
               "length": 800000000,
               "radius": {"min": 1, "max": 24, "step": 1}},

Границы интервалов включены в интервал. Пользователю предоставляется возмонжость изменить интервалы параметров запуска

Например, "length": {"min": 256, "max": 131072, "step": "mult", "step_val": 2} позволяет запускать бенчмарк с размером массива, увеличивающимся каждый раз вдвое. 
Если в параметре step задано число, то шаг каждый раз увеличивается на это число.

Исправлять скрипт при необходимости запуска на каких-то определённых параметрах, без интервалов, не нужно. Достаточно добавить опцию при запуске скрипта

--force = "--length=80 --radius=3 --mode=0"

Подробнее о параметрах бенчмарка - ниже

Также в скрипт можно передать следующие параметры:

--compiler= - используемый компилятор (команда вызова соответствующего компилятора из командной строки). По умолчанию используется g++
(Для Kunpeng g++ необходимо предварительно выполнить необходимые export)

--threads-num= - число потоков, явно экспортируемых в переменную окружения перед запуском бенчмарка. 
По умолчанию используется число ядер на одном NUMA-сокете целевой машины (см. lscpu)

--sockets= - изменить число NUMA-сокетов для запуска бенчмарка. По умолчанию, 1

--profile - для сбора аппаратных метрик и построения roofline-модели.

###### mode в бенчмарках:

для matrix_mult доступны 0-5 режима умножения матриц, отличающиеся порядком вложенных циклов

для triada доступны 0-15 типы операций над массивами данных,  в PRE_RELEASE версии пока использовать только 0-3

для matrix_transp доступны 0-3 методы транспонирования, 0-1 - обычный, 2-3 - блочный

length - используется как размер матрицы/длина массива/число вершин в графе(степень 2)

radius - используется в stencil бенчмарках

##### НОВЫЕ ИЗМЕНЕНИЯ (LICH)

-v --vtune - запуск vtune внутри makefile

--small --medium --large - выбор предустановленного режима запуска

внутри --force можно прописать --repeats=33, иначе по умолчанию вызовется с 8 повторами, как в locutils_new/Makefile

OMP PLACES можно задавать через affinity путём передачи через флаг --affinity в скрипт
