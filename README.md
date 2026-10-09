# Pressure Control

Controlador PID com intertravamentos e identificação de modelo de primeira ordem a partir de medições do processo.

## Executar

Requisitos: C++20 e CMake.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
build/identify_process processo.csv 0.1 > resultado.json
```

## Funcionamento

A identificação estima polo, ganho, constante de tempo, offset e erro residual. Entrada: `entrada,saída` por amostra e intervalo em segundos. Dados insuficientes ou modelos instáveis são recusados. Aplicação em processo físico exige validação e ajustes do equipamento.

## Persistência de resultados

O arquivo de operações está em [vercel-home-telemetry-api.vercel.app](https://vercel-home-telemetry-api.vercel.app/laboratory.html?project=pid-pressure-control). As migrações Supabase estão no [repositório da API](https://github.com/brunnojob/vercel-home-telemetry-api/tree/main/supabase/migrations).

```sh
python cloud/sync.py enqueue resultado.json --project pid-pressure-control
python cloud/sync.py sync
```

Defina `BRUNNODEV_ACCESS_TOKEN` com sua sessão. A fila SQLite conserva os relatórios até confirmação do servidor; o mesmo conteúdo não gera registros duplicados. Tokens não são gravados no código.
