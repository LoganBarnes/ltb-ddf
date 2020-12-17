Ltb Networking
==============
[![Travis CI][travis-badge]][travis-link]
[![Codecov][codecov-badge]][codecov-link]
[![MIT License][license-badge]][license-link]
[![Docs][docs-badge]][docs-link]

(Incomplete) C++ wrapper classes around [gRPC][grpc-link].

WARNING: Going through a major refactor right now. If you stumble
across this it's probably best to just keep on moving.

### Building

Requires the CMake build of gRPC 1.32.0 to be installed on the system.

### Development

```bash
# All from root dir or things may break

# Before all subtree commands
git remote add -f ltb-whatev git@github.com:LoganBarnes/ltb-whatev.git

# Subtree commands use form: subtree-name remote-name branch-name
git subtree add --prefix ltb-whatev ltb-whatev master --squash
git subtree push --prefix ltb-whatev ltb-whatev master
git subtree pull --prefix ltb-whatev ltb-whatev master --squash
```

[travis-badge]: https://travis-ci.org/LoganBarnes/ltb-net.svg?branch=master
[travis-link]: https://travis-ci.org/LoganBarnes/ltb-net
[codecov-badge]: https://codecov.io/gh/LoganBarnes/ltb-net/branch/master/graph/badge.svg
[codecov-link]: https://codecov.io/gh/LoganBarnes/ltb-net
[license-badge]: https://img.shields.io/badge/License-MIT-blue.svg
[license-link]: https://github.com/LoganBarnes/ltb-net/blob/master/LICENSE
[docs-badge]: https://codedocs.xyz/LoganBarnes/ltb-net.svg
[docs-link]: https://codedocs.xyz/LoganBarnes/ltb-net

[grpc-link]: https://grpc.io/
