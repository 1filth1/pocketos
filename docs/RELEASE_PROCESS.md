# Release Process

> This process is intentionally conservative while the build environment and target hardware are still being verified.

## 1. Prepare the candidate

- Select the exact source commit intended for the release.
- Review the open changes and update [PROJECT_STATUS.md](PROJECT_STATUS.md).
- Confirm the candidate version and write user-facing release notes.
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

## 4. Publish the release

Only after the previous steps are complete:

- Create a version tag using the agreed versioning scheme.
- Create a GitHub Release from the reviewed tag.
- Attach only artifacts built from that exact tag.
- Include checksums (for example SHA-256), build configuration and flashing instructions.
- Clearly label pre-release or experimental status where applicable.

## 5. After publication

- Keep the release notes and project status in sync.
- Track regressions and user reports through GitHub Issues.
- Do not silently replace a published binary; publish a corrected release with a new version and notes.

## Current release gate

PocketOS v0.1 is not considered a verified release until the build configuration is reproducible and the relevant hardware tests have been completed. The presence of a ZIP archive or source code alone does not satisfy this gate.

## Русский

> Пока среда сборки и целевое устройство не проверены, процесс выпуска намеренно остаётся консервативным.

### 1. Подготовка кандидата

- Выбрать точный коммит исходников для релиза.
- Проверить изменения и обновить [PROJECT_STATUS.md](PROJECT_STATUS.md).
- Подтвердить номер версии и подготовить Release Notes.
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
- Указать коммит прошивки, ревизию платы, дату и подтверждения.
- Описать известные проблемы, риски потери данных и восстановления.

### 4. Публикация релиза

Только после завершения предыдущих этапов:

- Создать тег версии по согласованной схеме.
- Создать GitHub Release на основе проверенного тега.
- Прикрепить только артефакты, собранные именно из этого тега.
- Добавить контрольные суммы (например SHA-256), конфигурацию сборки и инструкцию прошивки.
- Явно обозначить предварительный или экспериментальный статус, если он сохраняется.

### 5. После публикации

- Синхронизировать Release Notes и статус проекта.
- Отслеживать регрессии и сообщения пользователей через GitHub Issues.
- Не заменять опубликованный бинарник незаметно: исправление выпускается новой версией с отдельными заметками.

### Текущий критерий выпуска

PocketOS v0.1 нельзя считать проверенным релизом, пока конфигурация сборки не станет воспроизводимой и необходимые аппаратные тесты не будут выполнены. Одного ZIP-архива или исходников для этого недостаточно.
