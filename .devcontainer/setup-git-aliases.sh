#!/bin/bash

# Set global Git aliases
git config --local alias.st status
git config --local alias.co checkout
git config --local alias.br branch
git config --local alias.ci commit
git config --local alias.last "log -1 HEAD"
git config --local alias.amend "commit --amend"

echo "✅ Git aliases set."
