#!/bin/bash
cd ../testfiles
find . -type f -name *mapped* -print | xargs rm 
find . -type f -name *reduced* -print | xargs rm 
