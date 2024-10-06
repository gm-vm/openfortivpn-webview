Application to perform the SAML single sing-on and easily retrieve the
`SVPNCOOKIE` needed by `openfortivpn`.

The application will simply open the SAML page to let you sign in.
As soon as the `SVPNCOOKIE` is set, the application will print it to
stdout and exit.


## How to build

The project requires the Qt framework to be installed and it has
been tested with Qt 6.4.2.

### With qmake
To build the application, go to the root of the project and run:
```shell
# If you installed Qt via a package manager, the binary may be qmake6.
qmake .
# There should now be a Makefile if qmake has succeeded
make
```

The above should generate the `openfortivpn-webview` executable.

### With cmake
Qt6 has switched over to using `cmake` to build itself.  If you 
only have `cmake` available, build using:
```shell
cmake .
# There should now be a Makefile
make
```

