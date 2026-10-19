# ps5-remote-control-ios

App para iPhone que funciona como um controle de PS5 na tela. Em retrato, um controle para uma mão, com o que os
apps de streaming usam; em paisagem, o DualSense inteiro. Usa o protocolo do **Uso remoto** (Remote Play) pela
biblioteca aberta [chiaki-ng](https://github.com/chiaki-ng/chiaki-ng), na sua rede local, sem hardware extra.

É o mesmo controle do app do Mac (repositório irmão `ps5-remote-control-mac`, que também guarda o `ps5ctl`, a CLI de
testes, e o plano com as medições no PS5), com o mesmo motor: `shared/` é o mesmo código nos dois.

## Como funciona, em uma frase

O iPhone abre uma sessão de Uso remoto **sem áudio nem vídeo** só para mandar o estado do controle. O PS5 não deixa
assistir a serviços de vídeo durante o Uso remoto: com uma sessão aberta, a imagem dos apps de streaming fica escura
na TV (volta poucos segundos depois que a sessão fecha). Por isso o app abre a sessão no primeiro toque e a solta
sozinho quando você para de usar, mais rápido quando há um app de vídeo na frente.

## As telas

- **Retrato:** L2, L1, R1 e R2; direcional com ✕ no meio; ○ Voltar, PS e Início (PS segurado por 1,5 s); Create,
  Options, △ e □; embaixo, "▶ Assistir" (manda ✕ e solta a sessão na hora, para a imagem voltar) e a barra da TV.
- **Paisagem:** o DualSense inteiro: ombros, Create e Options, clique no touchpad, PS, Início, direcional, botões de
  ação e os dois analógicos (arrastar move; tocar é L3 ou R3).
- **Energia** (botão ⏻): ligar o PS5 pela rede, repouso, "Ligar tudo" e "Desligar tudo".
- **Ajustes:** console pareado, parear de novo, esquecer (pede confirmação: não tem volta), PIN de login do usuário
  do PS5, os tempos da conexão, o registro e o diagnóstico de rede.

Cada toque num botão manda um toque fixo de 80 ms (toques longos falham mais no console); o direcional repete
enquanto o dedo fica nele, e L2 e R2 seguram de verdade. O iPhone dá um toque leve (Taptic Engine) a cada botão e
outro quando a sessão conecta.

**Só toque.** O app obedece só ao toque na tela. Nenhuma tecla de um teclado físico vira botão do PS5 ou comando da
TV, em três camadas (`ios_app/Guard`): a `GuardedApplication` descarta toda tecla antes de qualquer tela (só as
folhas que coletam texto, como o código de pareamento, liberam o teclado); os controles não recebem foco de teclado;
e cada comando se recusa a executar se o evento que o iniciou foi uma tecla.

## Requisitos

- Mac com Apple Silicon, Xcode (testado com o 27.1), Homebrew e
  `brew install xcodegen cmake ninja pkgconf protobuf@29 openssl@3 libevent`.
- iPhone com iOS 18 ou mais novo, com o Modo de Desenvolvedor ligado, e uma conta Apple no Xcode. A gratuita serve:
  cada instalação vale 7 dias e cabem três apps assim por aparelho.
- PS5 na mesma rede Wi-Fi, com *Configurações › Sistema › Uso remoto › Ativar o Uso remoto* ligado.
- Para ligar o PS5 pelo app: *Configurações › Sistema › Economia de energia › Recursos disponíveis no modo de
  repouso*: **Continuar conectado à Internet** e **Habilitar opção de ligar o PS5 a partir da rede**.

Só para instalação pessoal pelo Xcode: a licença AGPL do chiaki-ng não convive com os termos da App Store.

## Compilar e instalar

```bash
git submodule update --init vendor/chiaki-ng                 # a biblioteca, em commit fixo
cp ios_app/Local.xcconfig.example ios_app/Local.xcconfig     # Team ID e bundle id próprios (ficam fora do Git)
scripts/build_chiaki_ios.sh                                  # a libchiaki para iPhone e Simulador (XCFramework)
scripts/build_ios.sh simulator                               # compila para o Simulador
scripts/build_ios.sh install --launch                        # compila, assina, instala e abre no iPhone
```

O `build_chiaki_ios.sh` compila o OpenSSL e a libevent a partir dos fontes que o Homebrew confere pelo hash e junta
tudo em `build/ios/ChiakiLib.xcframework` (só arm64: o Simulador precisa de um Mac com Apple Silicon). O
`build_ios.sh` desenha o ícone em código (`scripts/make_icon.swift`), gera o projeto do Xcode pelo XcodeGen
(`ios_app/project.yml`) e compila; `device` só compila e assina. O iPhone vem de `IOS_DEVICE_UDID`
(`xcrun devicectl list devices`) ou do arquivo `~/.config/ps5_remote_control/ios_device.json`, no formato
`{"udid": "..."}`, fora do repositório.

## Parear (uma vez)

O assistente abre sozinho na primeira vez, com a mesma sequência do Mac:

1. **Console.** O app procura o PS5 na rede. Na primeira busca o iOS pede a permissão de **Rede Local**: responda
   Permitir (se negar, *Ajustes › Privacidade e Segurança › Rede Local*).
2. **ID da conta PSN.** "Entrar na Sony" abre o login da Sony dentro do app, sem guardar a sessão, e o app pega o
   código do redirecionamento sozinho. Também dá para colar o ID pronto ou a URL em que a página termina.
3. **Código de 8 dígitos**, de *Configurações › Sistema › Uso remoto › Emparelhar dispositivo* no PS5.

O pareamento fica no Keychain, só neste aparelho (as chaves dão controle do PS5 a quem as tiver na rede local). Se o
PS5 pedir o PIN de login do usuário ao conectar, informe em Ajustes.

## Diferenças em relação ao Mac

Todas pelas regras de rede local do iOS (nota técnica TN3179 da Apple) ou pela tela de toque:

- **Sem broadcast.** Mandar broadcast exige uma permissão especial da Apple. O app pergunta ao endereço salvo e, se
  o console ficar em silêncio, sonda um a um os endereços da sub-rede (no máximo um /24); a busca automática faz no
  máximo uma varredura a cada 30 s. *Ajustes › Diagnóstico de rede* mede isso.
- **Só em primeiro plano.** Ao sair do app ou bloquear a tela, a sessão é solta; o primeiro toque reabre.
- **Pareamento no Keychain**, só neste aparelho (não sincroniza pelo iCloud nem volta num backup em outro aparelho).
- **Só toque**, no lugar do "só mouse" do Mac.
- **A TV ainda não.** No Mac, o volume, o mudo e o "Ligar tudo" pela TV LG vêm do projeto irmão `lg-roap-control`,
  que usa APIs só do macOS. O iPhone compila um substituto (`ios_app/TVStub`) até a etapa da TV: o volume e o mudo
  da barra da TV ficam desligados, o "Ligar tudo" só acorda o PS5 (a TV pode ligar junto pelo HDMI-CEC, se o Link
  de dispositivo HDMI do PS5 estiver ativo) e o "Desligar tudo" só põe o PS5 em repouso.

## Testes

`scripts/test_ios.sh` roda no Simulador, um teste de cada vez, num simulador próprio (`ps5-controle-teste`, iPhone 17
no iOS 27.0). Tudo em modo de teste: nenhum comando vai ao PS5 nem à TV, e o pareamento é de mentira, só em memória.

1. **Autoteste de teclas** dentro do app (`-ps5-selftest-input`): 52 eventos de tecla sintéticos pelo mesmo
   `sendEvent` do app. Em tela comum, todos são descartados; na folha de digitar, passam e nenhum comando roda
   durante eles; fora de evento de tecla, os mesmos comandos entram.
2. **Teste de toque** (XCUITest, `ios_app/Tests/UI`): cada toque real vira exatamente um comando.

O `typeKey` do Simulador não chega ao app como tecla física, então a prova com um teclado de verdade é no iPhone, com
um teclado Bluetooth.

## Estrutura

```
vendor/chiaki-ng/      biblioteca (submódulo em commit fixo)
scripts/               build_chiaki_ios (XCFramework), build_ios, test_ios, make_icon, verificar-privacidade
shared/Engine/         motor sem interface: descoberta, acordar, pareamento, sessão, estado do controle e política
shared/Theater/        HomeTheater: modos de toque, rota CEC, "Ligar tudo" e "Desligar tudo"
shared/UI/             o desenho do DualSense em SwiftUI
ios_app/project.yml    projeto do Xcode (XcodeGen); Local.xcconfig.example é o modelo do Local.xcconfig
ios_app/Platform/      entrada em UIKit, Keychain, permissão de Rede Local, ciclo de vida e retorno tátil
ios_app/Guard/         só toque: GuardedApplication e o autoteste de teclas
ios_app/UI/            retrato, paisagem, pareamento, login da Sony, ajustes e diagnóstico de rede
ios_app/TVStub/        substituto do cliente da TV até a etapa da TV
ios_app/CChiaki/       bridging header da biblioteca
ios_app/Tests/UI/      teste de toque (XCUITest)
```

## Privacidade

Nada neste repositório identifica o dono ou a casa: endereços de exemplo usam a faixa de documentação `192.0.2.x`,
MACs e nomes de console são marcadores, o pareamento fica só no Keychain do iPhone, e o Team ID e o bundle id
(`ios_app/Local.xcconfig`) e o identificador do iPhone ficam fora do Git. `scripts/verificar-privacidade.sh` confere
isso antes de cada commit e push; quem publica uma cópia deve manter, fora do repositório, uma segunda auditoria com
os próprios valores reais.

## Licença

Copyright (C) 2026 Controle PS5 contributors.

**AGPL-3.0-only**, com a permissão adicional para ligar com a OpenSSL ([LICENSE](LICENSE)). O código do protocolo vem
do [chiaki-ng](https://github.com/chiaki-ng/chiaki-ng), também AGPL-3.0: quem distribuir o app precisa oferecer o
código-fonte completo nos mesmos termos.
