# rsc_cite
Defines content types and blocks for citations (short quotes and paragraphs)

## What it does:

### rsc_cite.admin.inc

Defines the module settings page.

### rsc_cite.blocks.inc

- Defines blocks:
 - One that shows one random quote (short citation)
 - One that shows one random paragraph (long citation)
- Function that retrieves a random published node of the given content type.

### rsc_cite.common.inc

- Defines content type settings
- Defines fields and field instances

### rsc_cite.install

- Creates fields
- Sets up content type settings
- Sets up user role and permissions

### rsc_cite.module

- Define permissions
- Define menu callback
- Define content types
