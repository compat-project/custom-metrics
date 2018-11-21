MPWIDE CUSTOM METRIC
====================


This project is a custom metric for Arm Forge Professional (MAP) to record the communication metrics for the MPWide library.

LICENCE
=======

The code is licensed under the Apache License Version 2.0 -- see LICENSE-2.0.txt for the full text.

USAGE
=====

This custom metric depends on a modified version of MPWide -- included as a submodule.
Checkout the project with the `--recursive` flag and build the source code in the MPWide folder.

Update the Makefile to point to your correct map/metrics install directory, then run:
`make clean all install`

This should place the newly built metric in your local metrics dir `~/.allinea/map/metrics`.

Now when you run a MAP profile it should collect the extra MPWide communication data.

Note this metric also contains an Allinea Performance Reports Partial Reports, which will be installed by default, for presenting MPWide data in Performance Reports.


POC
===

Olly Perks `<olly.perks@arm.com>`
ComPat H2020 Project - No. 671564
