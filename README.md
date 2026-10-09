## Getting started

This repository uses [Unity](https://github.com/ThrowTheSwitch/Unity) (v2.7.0)
as a git submodule in `vendor/Unity`. Clone with submodules:

```bash
git clone --recurse-submodules https://github.com/jasond-embedded/stm32f410-drivers.git
```

If you already cloned without `--recurse-submodules`, `vendor/Unity` is empty.
Fetch it with:

```bash
git submodule update --init
```

`vendor/` is third-party code: never modify it. To change the Unity version,
check out another tag inside the submodule and commit the new reference.