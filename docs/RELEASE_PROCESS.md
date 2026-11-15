# Release Process

> This process is intentionally conservative while the build environment and target hardware are being verified.

## 1. Prepare the candidate

- Select the exact source commit intended for the release.
- Review the open changes and update [PROJECT_STATUS.md](PROJECT_STATUS.md).
- Update [CHANGELOG.md](../CHANGELOG.md) with the user-visible changes since the previous release.
- Create a version-specific `RELEASE_NOTES_<tag>.md` file using [RELEASE_NOTES_TEMPLATE.md](RELEASE_NOTES_TEMPLATE.md).
- Write the release description in both English and Russian. It must summarize the same changes as the changelog.
- Check that no credentials, private keys, personal data or local build artifacts are included.

## 2. Establish a reproducible build

- Build from a clean checkout or documented clean environment.
- Record the board definition, framework/toolchain versions and library dependencies.
- Record Flash size, PSRAM configuration and partition table.
- Save the complete build log and resulting artifact metadata.
- Do not publish a binary if the build cannot be reproduced.

## 3. Validate the hardware

- Use the exact target board revision.
- Follow the relevant cases in [TESTING.md](TESTING.md).
- Record PASS, FAIL, BLOCKED or NOT RUN honestly.
- Include the firmware commit, board revision, test date and evidence.
- Document known issues and any data-loss or recovery risks.
- A successful boot smoke test must not be described as full peripheral validation.

## 4. Publish the release

Only after the previous steps are complete:

- Create a version tag using the agreed versioning scheme.
- Push the tag to trigger the release workflow.
- The workflow must require `RELEASE_NOTES_<tag>.md` and a matching version heading in `CHANGELOG.md`; it must fail closed if either is missing.
- Create a GitHub Release from the reviewed tag.
- Attach only artifacts built from that exact tag.
- Include checksums (for example SHA-256), build configuration and flashing instructions.
- Clearly label pre-release or experimental status where applicable.
- Ordinary pushes to `release/pocketos-1.0` may build and validate artifacts, but must not publish or overwrite a versioned release or replace the deployed Web Flasher package.

## 5. Release notes format

Every release and pre-release must include all of the following, in English and Russian:

1. **Added / Добавлено** — new user-visible features.
2. **Fixed / Исправлено** — bugs fixed, with symptoms or root cause where known.
3. **Changed / Изменено** — behavior, interface, performance or configuration changes.
4. **Known issues / Известные проблемы** — incomplete or unverified functionality.
5. **Build & package / Сборка и пакет** — exact board, Flash/PSRAM profile, partition layout and source commit.
6. **Upgrade / Обновление** — installation steps, backup warning and any migration notes.

Do not list planned work as implemented. Do not claim hardware validation without recorded evidence. Keep the release notes, GitHub Release body and changelog consistent.

## 6. After publication

- Keep the release notes and project status in sync.
- Track regressions and user reports through GitHub Issues.
- Do not silently replace a published binary; publish a corrected release with a new version and notes.

## Current release gate

PocketOS is not considered fully hardware-validated until the relevant tests in [TESTING.md](TESTING.md) have been completed on the target board. A successful CI build, matching package hashes and deployed Web Flasher authorize a controlled test only; they do not prove hardware compatibility.

## Русский

> Пока среда сборки и целевое устройство проверяются, процесс выпуска намеренно остаётся консервативным.

### 1. Подготовка кандидата

- Выбрать точный commit исходников для релиза.
- Проверить изменения и обновить [PROJECT_STATUS.md](PROJECT_STATUS.md).
- Дополнить [CHANGELOG.md](../CHANGELOG.md) пользовательскими изменениями относительно предыдущего релиза.
- Создать отдельный файл `RELEASE_NOTES_<tag>.md` по [шаблону Release Notes](RELEASE_NOTES_TEMPLATE.md).
- Подготовить описание релиза на русском и английском языках. В обоих вариантах должны быть одинаковые по смыслу изменения.
- Убедиться, что в репозиторий не попали пароли, ключи, личные данные и локальные артефакты сборки.

### 2. Воспроизводимая сборка

- Выполнить сборку из чистого клона или документированной чистой среды.
- Записать модель платы, версии framework/toolchain и зависимости.
- Зафиксировать объём Flash, конфигурацию PSRAM и таблицу разделов.
- Сохранить полный лог сборки и сведения о полученном бинарнике.
- Не публиковать бинарник, если сборку нельзя воспроизвести.

### 3. Проверка на устройстве

- Использовать точную ревизию целевой платы.
- Выполнить подходящие проверки из [TESTING.md](TESTING.md).
- Честно записать PASS, FAIL, BLOCKED или NOT RUN.
- Указать commit прошивки, ревизию платы, дату и подтверждения.
- Описать известные проблемы, риски потери данных и восстановления.
- Успешный первичный запуск нельзя выдавать за полную проверку всей периферии.

### 4. Публикация релиза

Только после завершения предыдущих этапов:

- Создать тег версии по согласованной схеме.
- Отправить тег в GitHub — он запускает процесс публикации.
- Workflow обязан проверить наличие `RELEASE_NOTES_<tag>.md` и соответствующего заголовка версии в `CHANGELOG.md`; при отсутствии любого из них публикация должна завершиться ошибкой.
- Создать GitHub Release на основе проверенного тега.
- Прикрепить только артефакты, собранные именно из этого тега.
- Добавить контрольные суммы SHA-256, конфигурацию сборки и инструкцию прошивки.
- Явно обозначить предварительный или экспериментальный статус, если он сохраняется.
- Обычные push в `release/pocketos-1.0` могут собирать и проверять артефакты, но не должны публиковать или перезаписывать версионный релиз либо заменять пакет Web Flasher.

### 5. Формат Release Notes

Каждый релиз и пререлиз обязан содержать следующие разделы на двух языках:

1. **Added / Добавлено** — новые пользовательские функции.
2. **Fixed / Исправлено** — исправленные ошибки, по возможности с симптомами и первопричиной.
3. **Changed / Изменено** — изменения поведения, интерфейса, производительности или конфигурации.
4. **Known issues / Известные проблемы** — незавершённые и непроверенные функции.
5. **Build & package / Сборка и пакет** — точная плата, профиль Flash/PSRAM, разметка и исходный commit.
6. **Upgrade / Обновление** — установка, резервное копирование и особенности миграции.

Нельзя выдавать планы за реализованные функции или заявлять об аппаратной проверке без подтверждающих результатов. Release Notes, описание GitHub Release и CHANGELOG должны совпадать по смыслу.

### 6. После публикации

- Синхронизировать Release Notes и статус проекта.
- Отслеживать регрессии и сообщения пользователей через GitHub Issues.
- Не заменять опубликованный бинарник незаметно: исправление выпускается новой версией с отдельными заметками.

### Текущий критерий релиза

PocketOS нельзя считать полностью проверенной на железе, пока необходимые тесты из [TESTING.md](TESTING.md) не выполнены на целевой плате. Успешная CI-сборка, совпадающие контрольные суммы и опубликованный Web Flasher разрешают контролируемую пробную прошивку, но не доказывают аппаратную совместимость.
