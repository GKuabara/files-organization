# Brazilian Portuguese Version

# Projeto 1 - Funções SQL em C

Projeto da Disciplina de Organização de Arquivos da Universidade de São Paulo

## Estudantes
<a href="https://github.com/GKuabara"><img src="https://avatars.githubusercontent.com/u/66877791?v=4" title="gkuabara" width="80" height="80"></a>
<a href="https://github.com/milenacsilva"><img src="https://avatars2.githubusercontent.com/u/61664263?s=460&v=4" title="milenacsilva" width="80" height="80"></a>

Gabriel Alves Kuabara - nUSP 11275043 - gabrielalveskuabara@usp.br<br/>
Milena Corrêa da Silva - nUSP 11795401 - milenacorreasilva@usp.br

# Introdução

Basicamente, nosso programa consiste em funcionalidades da linguagem SQL construídas em C. Neste projeto, há 8 funcionalidades que funcionam com arquivos csv ou/e binários. Metade desses arquivos contém informações sobre a frota de ônibus da cidade de Curitiba e a outra metade contém informações sobre as linhas com as quais cada um desses ônibus está relacionado.

Os arquivos csv e binários têm um cabeçalho contendo informações gerais sobre o arquivo, sobre registros e nomes de cada campo além de registros com o conteúdo dos veículos e linhas. Em resumo, eles estão estruturados de acordo com as seguintes imagens:

- Cabeçalho do Veículo<br/>
![Vehicle Header](https://i.imgur.com/c8rAhVH.png)

- Registro do Veículo<br/>
![Vehicle Registers](https://i.imgur.com/OpMrJwS.png)

- Cabeçalho da Linha<br/>
![Line Header](https://i.imgur.com/erDIblB.png)

- Registro da Linha<br/>
![Line Registers](https://i.imgur.com/2x3xAJt.png)

# Planejamento do Projeto

Neste projeto, priorizamos o uso de memória em disco, não de memória RAM. Assim, tentamos usar o mínimo de memória principal, mantendo nossos arquivos atualizados com todas as informações que lemos ou gravamos.

## Modularização do código

Distribuímos nossas funcionalidades em seis arquivos C. O arquivo **operantions.c*** contém as principais funções de cada funcionalidade de nosso programa, em outras palavras, ele contém a "macro" de nosso programa. No arquivo **vehicle.c**, temos as funções relacionadas apenas com a estrutura dos arquivos do veículo. No **line.c**, temos as funções relacionadas apenas com a estrutura dos arquivos de linha. Portanto, no **global.c**, temos as funções que podem ser usadas tanto pelos arquivos de veículo quanto pelos arquivos de linha. Estas são principalmente funções que tratam partes comuns de ambas as estruturas de arquivos como os identificadores da remoção lógica e do tamanho do registro no início de cada registro de dados. Além disso, o **stream.c** trata principalmente da divisão de strings, abertura de arquivos e leitura de dados. Por fim, o **funcao-fornecida.c*** tem duas funções oferecidas pelos instrutores da disciplina. O primeiro é usado para verificar se o arquivo binário foi escrito corretamente e o segundo é usado para ler uma entrada com aspas no terminal. 

## Funcionalidades

### 1. Vehicle Create Table

A partir do arquivo csv do veículo, precisamos escrever um arquivo binário com as especificações dadas. Para cumprir nossa tarefa, abrimos os arquivos tratando possíveis erros. Então, iniciamos nosso cabeçalho com valores padrão, e começamos a ler (usando um lexer para particionar a linha) e escrever (usando múltiplas funções para escrever cada campo) cada registro do csv no binário. Após isso, atualizamos nosso cabeçalho com as informações obtidas a partir da inserção de todos esses registros.

### 2. Line Create Table

A partir do arquivo csv de linha, precisamos escrever um arquivo binário com as especificações dadas. Para cumprir nossa tarefa, abrimos os arquivos tratando possíveis erros. Então, iniciamos nosso cabeçalho com valores padrão, e começamos a ler (usando um lexer para particionar a linha) e escrever (usando múltiplas funções para escrever cada campo) cada registro do csv para o binário. Após isso, atualizamos nosso cabeçalho com as informações obtidas a partir da inserção de todos esses registros.

### 3. Vehicle Select

Do arquivo binário do veículo, lemos todos os registros válidos (aqueles que não são removidos) e os imprimimos conforme solicitado. Primeiro, verificamos a consistência de nosso arquivo binário, e depois lemos cada registro (armazenado em uma struct) até o último byte do arquivo, imprimindo todos os registros **não*** removidos. Se não encontramos nenhum registro, imprimimos uma mensagem de erro.

### 4. Line Select

Do arquivo binário de linha, lemos todos os registros válidos (aqueles que não são removidos) e os imprimimos conforme solicitado. Primeiro, verificamos a consistência de nosso arquivo binário, e depois lemos cada registro (armazenado em uma struct) até o último byte do arquivo, imprimindo todos os registros **não*** removidos. Se não encontramos nenhum registro, imprimimos uma mensagem de erro.

### 5. Vehicle Select Where

A partir do arquivo binário do veículo, precisamos procurar e imprimir todos os registros que contenham um valor específico em um campo específico dado pelo usuário. Começamos verificando a consistência de nosso arquivo, seguindo pesquisando em cada registro se ele contém o "valor" no "campo" solicitado. Assim, imprimimos todos os registros que contêm o "valor" no "campo". Se houver nenhum registro, o programa imprime uma mensagem de erro.

### 6. Line Select Where

A partir do arquivo binário de linha, precisamos procurar e imprimir todos os registros que contenham um valor específico em um campo específico dado pelo usuário. Começamos verificando a consistência de nosso arquivo, seguindo pesquisando em cada registro se ele contém o "valor" no "campo" solicitado. Assim, imprimimos todos os registros que contêm o 'valor' no 'campo'. Se houver nenhum registro, o programa imprime uma mensagem de erro.

### 7. Vehicle Insert Into

O programa recebe 'n' registros digitados pelo usuário que precisamos adicionar no final do arquivo binário do veículo. Antes de mais nada, verificamos a consistência de nosso arquivo. Em seguida, lemos cada linha/registo do terminal, adicionando-os ao final do arquivo. Enfim, atualizamos o cabeçalho dado o número de novos registros inseridos.

### 8. Line Insert Into

O programa recebe 'n' registros digitados pelo usuário que precisamos adicionar no final da linha do arquivo binário. Antes de mais nada, verificamos a consistência de nosso arquivo. Em seguida, lemos cada linha/registro do terminal, adicionando-os ao final do arquivo. Enfim, atualizamos o cabeçalho dado o número de novos registros inseridos.

# Instruções de uso

Para executar nosso programa, usamos um makefile. Usando o comando "Make" compilamos e limpamos arquivos objeto, então podemos executar nosso programa através do arquivo executável "./main". Em seguida, o usuário precisa digitar o número da funcionalidade e os parâmetros de cada um deles, separados por espaços. 

- As funcionalidades 1 e 2 recebem o csv e o nome do arquivo binário respectivamente:<br/>
![](https://i.imgur.com/OOqsNlL.png)<br/>
![](https://i.imgur.com/wFpkPIG.png)

- As funcionalidades 3 e 4 recebem o nome do arquivo binário:<br/>
![](https://i.imgur.com/qY02otn.png)<br/>
![](https://i.imgur.com/lKOkhuL.png)

- As funcionalidades 5 e 6 recebem o nome do arquivo binário, um campo de arquivo e o valor que o usuário está procurando:<br/>
![](https://i.imgur.com/BGh32d2.png)<br/>
![](https://i.imgur.com/zBSimc7.png)

- As funcionalidades 7 e 8 recebem o nome do arquivo binário e o número de registros a serem inseridos. Depois disso, em cada linha é digitado um registro que tem strings entre aspas:<br/>
![](https://i.imgur.com/AxdRuEy.png)<br/>
![](https://i.imgur.com/W084Wmz.png)


_____________________________________________________________________________________________________________________


# English Version

# Project 1 - SQL Functions in C

Project from Files Organization Discipline of University of São Paulo

## Students

Gabriel Alves Kuabara - nUSP 11275043 - gabrielalveskuabara@usp.br<br/>
Milena Corrêa da Silva - nUSP 11795401 - milenacorrea@usp.br

# Introduction

Basically, our program consists of SQL language functionalities built in C. In this project, there are 8 functionalities that works with csv or/and binary files. Half of these files contains information about the bus fleet of the city of Curitiba and the another half contains information about the lines which each of these buses are related.

The csv and binary files have a header containing general information about the file, about registers, and names of each field besides registers with the content of vehicles and lines. In summary, they are structured in according with the following images:

- Vehicle Header<br/>
![Vehicle Header](https://i.imgur.com/c8rAhVH.png)

- Vehicle Register<br/>
![Vehicle Registers](https://i.imgur.com/OpMrJwS.png)

- Line Header<br/>
![Line Header](https://i.imgur.com/erDIblB.png)

- Line Register<br/>
![Line Registers](https://i.imgur.com/2x3xAJt.png)

# Project Planning

In this project, we may prioritize the use of disk memory, not of RAM memory. So we tried to use the minimun of main memory by always maintaining our files updated with every information that we read or write.

## Code Modularization

We distributed our functionalities over six C files. The file **operantions.c** contains the main functions of each functionality of our program, in other words, it contains the "macro" of our program. In the **vehicle.c**, we have the functions related only about the vehicle files structure. In the **line.c**, we have the functions related only about the line files structure. So, in the **global.c** we have functions that can be used by both vehicle and line files. These are mainly functions that treat common parts of both files structures like the identifiers of logic remotion and of register size in the beggining of every data register. Also, the **stream.c** deals mainly with spliting strings, openning files, and readline. At least, the **funcao-fornecida.c** have two funcions offered by the instructors of the discipline. The first one is used to check if the binary file was written correctly and the second is used to read an input with quotes from the terminal. 

## Functionalities

### 1. Vehicle Create Table

From the vehicle csv file, we need to write a binary file with the given specifications. To accomplish our task, we fisrt open the files, treating possible erros. Then, we start our header with standard values, and start to read (using a readline and string lexer) and write (using multiple functions to fwrite each field) each register from the csv into the binary. After it, we update our header with the information got from the insertion of all these register.

### 2. Line Create Table

From the line csv file, we need to write a binary file with the given specifications. To accomplish our task, we fisrt open the files, treating possible erros. Then, we start our header with standard values, and start to read (using a readline and string lexer) and write (using multiple functions to fwrite each field) each register from the csv into the binary. After it, we update our header with the information got from the insertion of all these register.

### 3. Vehicle Select

From the vehicle binary file, we read all the valid register (those which aren't removed) and print them as requested. Firstly, we check the consistency of our binary file, and then read each register (storing into a struct) until the file last byte, printing all **not** removed registers. If we didn't find any register, we print a message error.

### 4. Line Select

From the line binary file, we read all the valid register (those which aren't removed) and print them as requested. Firstly, we check the consistency of our binary file, and then read each register (storing into a struct) until the file last byte, printing all **not** removed registers. If we didn't find any register, we print a message error.

### 5. Vehicle Select Where

From the vehicle binary file, we need to search and print for all the registers containing a specific value in a specific field given by the user. We start by checking our file consistency, following by searching in each register if it contains the 'value' in the requested 'field'. So, we print all the registers containing the 'value' in the 'field'. If there is any register, the program prints a message error.

### 6. Line Select Where

From the line binary file, we need to search and print for all the registers containing a specific value in a specific field given by the user. We start by checking our file consistency, following by searching in each register if it contains the 'value' in the requested 'field'. So, we print all the registers containing the 'value' in the 'field'. If there is any register, the program prints a message error.

### 7. Vehicle Insert Into

The program receives 'n' registers typed by the user that we need to add in the end of the vehicle binary file. First of all, we check the consistency of our file. Following, we read each line/register from the terminal, adding them to the end of the file. At least, we update the header given the number of new inserted register.

### 8. Line Insert Into

The program receives 'n' registers typed by the user that we need to add in the end of the line binary file. First of all, we check the consistency of our file. Following, we read each line/register from the terminal, adding them to the end of the file. At least, we update the header given the number of new inserted register.

# Use Instructions

To execute our program we use a makefile. By using command "Make" we compile and clean object files, then we can execute our program through the executable main file "./main". Then, the user need to type the funcionality number and the parameters of each one separeted by spaces. 

- Functionalities 1 and 2 receives the csv and binary file name respectively:<br/>
![](https://i.imgur.com/OOqsNlL.png)<br/>
![](https://i.imgur.com/wFpkPIG.png)

- Functionalities 3 and 4 receives the binary file name:<br/>
![](https://i.imgur.com/qY02otn.png)<br/>
![](https://i.imgur.com/lKOkhuL.png)

- Functionalities 5 and 6 receives the binary file name, a file field, and the value the user is searching for:<br/>
![](https://i.imgur.com/BGh32d2.png)<br/>
![](https://i.imgur.com/zBSimc7.png)

- Functionalities 7 and 8 receives the binary file name and the number of registers to be inserted. After this, in each line is typed one register which have its strings between quotes:<br/>
![](https://i.imgur.com/AxdRuEy.png)<br/>
![](https://i.imgur.com/W084Wmz.png)
