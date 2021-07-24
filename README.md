# Brazilian Portuguese Version

# Projeto - Funções SQL em C

Projeto da Disciplina de Organização de Arquivos da Universidade de São Paulo

## Estudantes
<a href="https://github.com/GKuabara"><img src="https://avatars.githubusercontent.com/u/66877791?v=4" title="gkuabara" width="80" height="80"></a>
<a href="https://github.com/milenacsilva"><img src="https://avatars2.githubusercontent.com/u/61664263?s=460&v=4" title="milenacsilva" width="80" height="80"></a>

Gabriel Alves Kuabara - nUSP 11275043 - gabrielalveskuabara@usp.br<br/>
Milena Corrêa da Silva - nUSP 11795401 - milenacorreasilva@usp.br

# Introdução

Basicamente, nosso programa consiste em funcionalidades da linguagem SQL construídas em C. Neste projeto, há 19 funcionalidades que funcionam com arquivos de dados csv e binários, e arquivos de índices árvore-B. Metade desses arquivos contém informações sobre a frota de ônibus da cidade de Curitiba e a outra metade contém informações sobre as linhas com as quais cada um desses ônibus está relacionado.

Os arquivos csv e binários têm um cabeçalho contendo informações gerais sobre o arquivo, sobre registros e nomes de cada campo além de registros com o conteúdo dos veículos e linhas. Em resumo, eles estão estruturados de acordo com as seguintes imagens:

- Cabeçalho do Veículo<br/>
![Vehicle Header](https://i.imgur.com/c8rAhVH.png)

- Registro do Veículo<br/>
![Vehicle Registers](https://i.imgur.com/OpMrJwS.png)

- Cabeçalho da Linha<br/>
![Line Header](https://i.imgur.com/erDIblB.png)

- Registro da Linha<br/>
![Line Registers](https://i.imgur.com/2x3xAJt.png)

Já os arquivos de índices, são organizados de acordo com uma estrutura de dados árvore-B de ordem 5. Portanto, temos que cada página/nó contém 5 ponteiros(RRNs) para outras páginas e 4 chaves, que identificam cada uma, um offset do registro de dados de algum arquivo de dados binário:<br/><br/>
![](https://i.imgur.com/7XDQL3t.png)<br/>
![](https://i.imgur.com/D5qbuxq.png)

# Planejamento do Projeto

Neste projeto, priorizamos o uso de memória em disco, não de memória RAM. Assim, tentamos usar o mínimo de memória principal, mantendo nossos arquivos atualizados com todas as informações que lemos ou gravamos.

## Modularização do código

Distribuímos nossas funcionalidades em dez arquivos C. O arquivo **operantions.c** contém as principais funções de cada funcionalidade de nosso programa, em outras palavras, ele contém o "macro" de nosso programa. O arquivo **aux.c** contém as funções auxiliares das funções principais de cada funcionalidade presente em operations.c. No arquivo **vehicle.c**, temos as funções relacionadas apenas com a estrutura dos arquivos do veículo. No **line.c**, temos as funções relacionadas apenas com a estrutura dos arquivos de linha. Portanto, no **global.c**, temos as funções que podem ser usadas tanto pelos arquivos de veículo quanto pelos arquivos de linha. Estas são principalmente funções que tratam partes comuns de ambas as estruturas de arquivos como os identificadores da remoção lógica e do tamanho do registro no início de cada registro de dados. Além disso, o **stream.c** trata principalmente da divisão de strings e leitura de dados do terminal. O arquivo **file_handler.c** cuida do tratamento de arquivos. O arquivo **funcao-fornecida.c** tem três funções oferecidas pelos instrutores da disciplina. A primeira é usada para verificar se o arquivo binário foi escrito corretamente, a segunda é usado para ler uma entrada com aspas no terminal e a terceira para converter uma chave de formato "string" em formato númerico. O arquivo **btree.c** contém as funções relacionadas a tal estrutura de dados. Por fim, o arquivo **fieldcmp.c** contém as funções de comparação que são usadas para ordenação dos registro usando qsort.

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

### 9. Vehicle Create Index

Dado um arquivo de dados binário veiculo, criado pela funcionalidade 1, criamos um arquivo de índices árvore-B (ordem 5), indexado pelo prefixo dos registros no arquivo de dados veiculo usando a função "convertePrefixo" disponibilizada pela docente. Assim, fazemos a inserção de cada registro um por vez. Após a inserção de todos, atualizamos o cabeçalho do arquivo de índice.

### 10. Line Create Index

Dado um arquivo de dados binário linha, criado pela funcionalidade 1, criamos um arquivo de índices árvore-B (ordem 5), indexado pelo prefixo dos registros no arquivo de dados linha usando a função "convertePrefixo" disponibilizada pela docente. Assim, fazemos a inserção de cada registro um por vez. Após a inserção de todos, atualizamos o cabeçalho do arquivo de índice.

### 11. Vehicle Search Key

Do arquivo de índice veículo, faz-se uma busca pela árvore-B por uma chave, no caso dos arquivos referentes ao veículo, essa chave é o campo prefixo (unicamente identificado). Após a busca, caso existir a requisitada chave na árvore, temos em mãos o offset para o registro no arquivo de dados. Então, acessamos o arquivos e imprimimos as informações do registro.

### 12. Line Search Key

Do arquivo de índice linha, faz-se uma busca pela árvore-B por uma chave, no caso dos arquivos referentes ao linha, essa chave é o campo prefixo (unicamente identificado). Após a busca, caso existir a requisitada chave na árvore, temos em mãos o offset para o registro no arquivo de dados. Então, acessamos o arquivos e imprimimos as informações do registro.

### 13. Vehicle Insert Key

Extensão da Funcionalidade 7, após a inserção de algum registro no arquivos de dados, devemos fazer a inserção da chave que identifica esse registro (prefixo) no arquivo de índice também.

### 14. Line Insert Key

Extensão da Funcionalidade 8, após a inserção de algum registro no arquivos de dados, devemos fazer a inserção da chave que identifica esse registro (codLinha) no arquivo de índice também.

### 15. Select From

A partir dos arquivos binários contendo os registros, tem-se um loop pelo arquivo de veiculo comparando cada um dos seus registros com cada registro do arquivo de linha, verificando se o campo "linha" bate com o campo "codLinha". Caso bater, então imprime as informações do registro de veiculo e linha, respectivamente.

### 16. Select From Index

A partir do arquivo binário de veículo, faz-se um loop por cada registro, no qual usa-se o campo "linha" para procurar um valor igual no arquivo de índice linha árvore-B, indicando que há um registro no arquivo linha com o mesmo valor. Então, carrega-se esse registro linha para memória e imprime o registro veículo e linha, respectivamente.

### 17. Vehicle Sort File

Carrega-se para memória, todos os registros do arquivo binário veículo, fazendo a ordenação com o **qsort**. Depois, escrevemos todos os registros ordenados num novo arquivo.

### 18. Line Sort File

Carrega-se para memória, todos os registros do arquivo binário linha, fazendo a ordenação com o **qsort**. Depois, escrevemos todos os registros ordenados num novo arquivo.

### 19. Merge Files

Tendo ambos arquivos binários ordenados atráves das chamadas das funcionalidades 17 e 18, faz-se uma junção de ambos, atráves da impressão dos registros que combinam os campos "linha"(veículo) e "codLinha"(linha).

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

- As funcionalidades 9 e 10 recebem o nome do arquivo binário e do arquivo de índice a ser criado:<br/>
![](https://i.imgur.com/V0x1QeD.png)<br/>
![](https://i.imgur.com/nnsIVUT.png)

- As funcionalidades 11 e 12 recebem o nome do arquivo binário, nome arquivo de índice, o campo a ser pesquisado e a chave que identifica algum registro:<br/>
![](https://i.imgur.com/XX8ZM9N.png)<br/>
![](https://i.imgur.com/cNhLgNq.png)

- As funcionalidades 13 e 14 recebem o nome do arquivo binário, nome arquivo de índice e o número de registros a serem inseridos no arquivo de dados. Em seguida é digitado as informações de cada registro a ser inserido:<br/>
![](https://i.imgur.com/XEAl7gw.png)<br/>
![](https://i.imgur.com/B1m3pa9.png)

- A funcionalidade 15 recebe os nomes dos árquivos binários e os campos que serão usados para comparação, no caso somente o "codLinha" é válido neste trabalho:<br/>
![](https://i.imgur.com/a7iWofC.png)

- A funcionalidade 16 recebe os nomes dos árquivos binários, os campos que serão usados para comparação, e o nome do arquivo de índice, no caso somente o campo "codLinha" é válido neste trabalho:<br/>
![](https://i.imgur.com/Zqd5udo.png)

- A funcionalidade 17 e 18 recebem o nome do árquivo binário, o nome do arquivo a ser criado com registros ordenados e o campo que deve ser usado para fazer a comparação na ordenação:<br/>
![](https://i.imgur.com/lZb7kAD.png)
![](https://i.imgur.com/hJJmpCP.png)

- A funcionalidade 19 recebe os nomes dos arquivos binários e os campos que devem ser usados para comparação:<br/>
![](https://i.imgur.com/j0PT0Zj.png)

_____________________________________________________________________________________________________________________


# English Version

# Project - SQL Functions in C

Project from Files Organization Course of University of São Paulo

## Students
<a href="https://github.com/GKuabara"><img src="https://avatars.githubusercontent.com/u/66877791?v=4" title="gkuabara" width="80" height="80"></a>
<a href="https://github.com/milenacsilva"><img src="https://avatars2.githubusercontent.com/u/61664263?s=460&v=4" title="milenacsilva" width="80" height="80"></a>

Gabriel Alves Kuabara - nUSP 11275043 - gabrielalveskuabara@usp.br<br/>
Milena Corrêa da Silva - nUSP 11795401 - milenacorreasilva@usp.br

# Introduction

Basically, our program consists of SQL language functionalities built in C. In this project there are 19 functionalities that work with csv and binary data files, and B-tree index files. Half of these files contain information about the bus fleet of the city of Curitiba, and the other half contain information about the lines to which each of these buses is related.

The csv and binary files have a header containing general information about the file, about registers, and names of each field besides registers with the content of vehicles and lines. In summary, they are structured in according with the following images:

- Vehicle Header<br/>
![Vehicle Header](https://i.imgur.com/c8rAhVH.png)

- Vehicle Register<br/>
![Vehicle Registers](https://i.imgur.com/OpMrJwS.png)

- Line Header<br/>
![Line Header](https://i.imgur.com/erDIblB.png)

- Line Register<br/>
![Line Registers](https://i.imgur.com/2x3xAJt.png)

The index files, on the other hand, are organized according to a B-tree data structure of order 5. So we have that each page/node contains 5 pointers(RRNs) to other pages and 4 keys that identify the data record's offset from some binary data file:<br/><br/>

![](https://i.imgur.com/7XDQL3t.png)<br/>
![](https://i.imgur.com/D5qbuxq.png)

# Project Planning

In this project, we may prioritize the use of disk memory, not of RAM memory. So we tried to use the minimun of main memory by always maintaining our files updated with every information that we read or write.

## Code Modularization

We have distributed our functionality into ten C-files. The file **operations.c** contains the main functions of each functionality of our program, in other words, it contains the "macro" of our program. The file **aux.c** contains the auxiliary functions for the main functions of each feature present in operations.c. In the **vehicle.c** file, we have the functions related only to the structure of the vehicle files. In the **line.c**, we have the functions related only to the structure of the line files. So in **global.c**, we have the functions that can be used by both the vehicle files and the line files. These are mainly functions that handle common parts of both file structures such as the logical removal and record size identifiers at the beginning of each data record. In addition, **stream.c** mainly handles string splitting and reading data from the terminal. The **file_handler.c** file takes care of file handling. The file **funcao_fornecida.c** has three functions provided by the discipline instructors. The first is used to check that the binary file has been written correctly, the second is used to read a quoted input in the terminal and the third to convert a key from string format to number format. The file **btree.c** contains the functions related to that data structure. Finally, the **fieldcmp.c** file contains the comparison functions that are used for sorting the records using qsort.

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

### 9. Vehicle Create Index

Given a binary vehicle data file, created by functionality 1, we create a B-tree index file (order 5), indexed by the prefix of the registers in the vehicle data file using the function "convertePrefixo" provided by the instructors. So we insert each record one at a time. After inserting them all, we update the header of the index file.

### 10. Line Create Index

Given a binary line data file, created by functionality 2, we create a B-tree index file (order 5), indexed by the codLinha of the registers in the line data file using the "convertePrefixo" function provided by the instructors. So we insert each record one at a time. After inserting them all, we update the header of the index file.

### 11. Vehicle Search Key

From the vehicle index file, we search a key through the B-tree, in the case of vehicle files, this key is the prefix field (uniquely identified). After the search, if the required key exists in the tree, we have the offset of the register in the data file. We then access the file and print out the register information.

### 12. Line Search Key

From the line index file, we search a key through the B-tree, in the case of row index files, this key is the codLinha field (uniquely identified). After the search, if the requested key exists in the tree, we have the offset of the register in the data file. We then access the file and print out the register information.

### 13. Veículo Insert Key

Extends functionality 7, after inserting some register in the data file, we must insert the key that identifies this register (prefix) in the index file as well.

### 14. Linha Insert Key

Extends functionality 8, after inserting some register in the data file, we must insert the key that identifies this register (codLinha) in the index file as well.

### 15. select from

From the binary files containing the records, there is a loop through the vehicle file comparing each of its records with each record in the line file, checking if the "line" field matches the "codLinha" field. If it matches, then prints the information of the vehicle and line records, respectively.

### 16. Select From Index

From the vehicle binary file, it loops through each record, in which it's used the "line" field to look for a matching value in the B-tree line index file, indicating that there is a record in the line file with the same value. It then loads that line record into memory and prints the vehicle and line record, respectively.

### 17. Vehicle Sort File

We load into memory all the records in the binary vehicle file, sorting them with the **qsort**. Then, we write all the sorted records to a new file.

### 18. Line Sort File

We load into memory, all the records of the binary line file, sorting them with the **qsort**. Then, we write all the sorted records to a new file.

### 19. Merge Files

Having both binary files sorted by calling functionalities 17 and 18, we merge them by printing the records that match the "line"(vehicle) and "codLinha"(line) fields.


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

- Functionalities 9 and 10 receives the binary file name and the index file name to be created:<br/>
![](https://i.imgur.com/V0x1QeD.png)<br/>
![](https://i.imgur.com/nnsIVUT.png)

- Functionalities 11 and 12 receives the binary file name, the index file name, the field to be searched, and the key that identifies some register:<br/>
![](https://i.imgur.com/XX8ZM9N.png)<br/>
![](https://i.imgur.com/cNhLgNq.png)

- Functionalities 13 and 14 receives the binary file name, the index file name, and the number of registers to be inserted into the data file. Then, the information for each register to be inserted is typed:<br/>
![](https://i.imgur.com/XEAl7gw.png)<br/>
![](https://i.imgur.com/B1m3pa9.png)

- Functionality 15 receives the binary files names and the fields that will be used to check if their content are equal, in this case only the "codLinha" is valid in this work:<br/>
![](https://i.imgur.com/a7iWofC.png)

- Functionality 16 receives the binary files names, the fields that will be used to check if their content are equal, and the name of the index file, in this case only the field "codLinha" is valid in this work:<br/>
![](https://i.imgur.com/Zqd5udo.png)

- Functionalities 17 and 18 are given the binary files names, the name of the file to be created with sorted records, and the field to be used to compare values in the sorting:<br/>
![](https://i.imgur.com/lZb7kAD.png)
![](https://i.imgur.com/hJJmpCP.png)

- Functionality 19 receives the names of the binary files and the fields that are to be used to check if their content are equal:<br/>
![](https://i.imgur.com/j0PT0Zj.png)