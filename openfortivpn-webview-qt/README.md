Application to perform the SAML single sing-on and easily retrieve the
`SVPNCOOKIE` needed by `openfortivpn`.

The application will simply open the SAML page to let you sign in.
As soon as the `SVPNCOOKIE` is set, the application will print it to
stdout and exit.


## How to build

The project requires the Qt framework to be installed and it has
been tested with Qt 6.4.2.

To build the application, go to the root of the project and run:
```shell
qmake .
# There should now be a Makefile if qmake has succeeded
make
```

The above should generate the `openfortivpn-webview` executable.
