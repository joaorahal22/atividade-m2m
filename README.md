# Atividade Machine2Machine (M2M)

**Aluno:** João Rahal  
**RA:** 20125654  
**Instituição:** PUC-Campinas

## Objetivo
Organizar dois projetos de comunicação entre ESP8266 via Wi-Fi e HTTP em um repositório GitHub e clonar esse repositório no ambiente AWS.

## Projetos
| Pasta | Funcionamento | Monitor serial |
|---|---|---|
| `ping_pong` | Recebe GET em `/toggle` e inverte o LED do GPIO 2. Envia GET ao outro ESP a cada 10 segundos. | 9600 baud |
| `telefone_sem_fio` | Recebe inteiro em `/relay?valor=0`, soma 1 e encaminha ao próximo ESP após 2 segundos. Valida o valor e recusa outra mensagem enquanto há envio pendente. | 115200 baud |

Cada pasta contém `mycode.ino` e `cpp/mycode.cpp`. O `.cpp` é uma versão alternativa do mesmo firmware, com inclusão explícita de `Arduino.h`. Compile apenas uma versão por projeto para evitar duplicação de `setup()` e `loop()`.

## Configuração e execução
1. Instale o suporte à placa ESP8266 na Arduino IDE e selecione sua placa e porta USB.
2. Copie o `mycode.ino` escolhido para uma pasta local chamada `mycode` e abra-o na IDE.
3. Configure o nome e a senha da rede Wi-Fi. A senha original foi substituída por `CONFIGURE_SUA_SENHA` nas versões publicadas.
4. No ping-pong, ajuste `IP_DO_COLEGA` e configure o outro ESP para apontar de volta para esta placa.
5. No telefone sem fio, ajuste IP local, gateway, máscara, DNS e IP do próximo ESP conforme a rede. Cada placa precisa de um IP próprio.
6. Grave o firmware e abra o monitor serial na velocidade indicada. Os dispositivos devem estar em uma rede que permita comunicação entre clientes.
7. Para iniciar o telefone sem fio, acesse `http://<IP_DO_ESP>/relay?valor=0`. Para testar o LED, acesse `http://<IP_DO_ESP>/toggle`.

As bibliotecas ESP8266WiFi, ESP8266WebServer e ESP8266HTTPClient pertencem ao suporte Arduino para ESP8266. Os arquivos `.cpp` também dependem desse ambiente; não são programas independentes para executar com g++ no servidor Linux.

## Clonagem na AWS
No terminal da instância AWS:

```bash
git clone https://github.com/joaorahal22/atividade-m2m.git
cd atividade-m2m
find . -maxdepth 3 -type f -not -path './.git/*'
```

Se a pasta já tiver sido clonada, use `git -C atividade-m2m pull --ff-only` a partir da pasta superior.

A AWS armazena a cópia do repositório nesta atividade. A execução dos firmwares e a validação física de LEDs e comunicação exigem placas ESP8266.
