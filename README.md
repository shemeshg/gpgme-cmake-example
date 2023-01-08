# gpgme-cmake-example
Very simple gpgme cmake example

## On macos

```
brew install gnu-getopt gnupg qrencode gpgme libgpg-error  pinentry-mac 
echo "pinentry-program /usr/local/bin/pinentry-mac" >> ~/.gnupg/gpg-agent.conf
gpgconf --kill gpg-agent

```
