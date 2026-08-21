# Contributing

## Workflow

1. Create or update files in the appropriate directory.
2. Keep file and folder names in lowercase kebab-case.
3. Run project checks before committing, when checks exist.
4. Review `git diff` before staging changes.
5. Commit with a concise conventional commit message.

## File Organization

- `src/`: source code
- `tests/`: test files
- `docs/`: project documentation
- `scripts/`: helper scripts

## Do Not Commit

- Secrets or private credentials
- Local `.env` files
- Dependency directories such as `node_modules/`
- Build output such as `dist/`, `build/`, or `coverage/`
