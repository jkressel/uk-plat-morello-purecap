# plat-morello

Morello platform

In order to compile an application that will run on the Morello, please
select/deselect the following options in the make menuconfig:
- Architecture Selection --> Architecture --> select: Arrmv8 compatible (64 bits)
- Architecture Selection --> Processor Optimization --> select: Generic Armv8 CPU
- Architecture Selection --> deselect: Workaround for Cortex-A73 erratum
- Platform Configuration --> select: Morello

This will build an ELF file. Once built, consult the ARM Morello bare metal documentation https://git.morello-project.org/morello/docs/-/blob/morello/mainline/standalone-baremetal-readme.rst

Please note, this is still very much an early, possibly horribly broken WIP. Most features are not yet present.
