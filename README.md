# FSE_PROJ2

## 1. Objetivos

Repositório voltado ao desenvolvimento de Projeto de Embarcados 2 - UnB 2021/2

### 1.1 Enunciado
Este trabalho tem por objetivo a criação de um sistema distribuído de automação predial para monitoramento e acionamento de sensores e dispositivos de um prédio de 2 andares. Para mais informações sobre o enunciado visite este <a href="https://gitlab.com/fse_fga/trabalhos-2021_2/trabalho-2-2021-2">link</a> 

A Figura abaixo mostra a arquitetura do sistema:

<img src="./images/arquitetura_projeto_2.png" alt="Arquitetura Projeto 2">

## 2. Funcionamento
Para executar o código é necessário o uso de dois terminais para executá-los nas placas RASP42 e RASP43


1. Clone o repositório com o comando:
```terminal
git clone 
```

2. Na placa RASP43 acesse o diretório do servidor central:
```terminal
cd ./FSE_PROJ2/central_server
```

Ja na placa RASP42 acesse o diretório do servidor distribuido
```terminal
cd ./FSE_PROJ2/distributed_server
```

3. Execute o make para compilar os arquivos em ambos os diretórios:
```terminal
make
```

4. Execute o comando abaixo em ambos os diretórios para rodar o projeto:
```terminal
bin/bin
```

