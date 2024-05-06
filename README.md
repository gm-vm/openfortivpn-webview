Application to perform the SAML single sign-on and easily retrieve the
`SVPNCOOKIE` needed by [`openfortivpn`](https://github.com/adrienverge/openfortivpn/).

The application will simply open the SAML page to let you sign in.
As soon as the `SVPNCOOKIE` is set, the application will print it to
stdout and exit.

The application comes in two flavors:
 - [openfortivpn-webview-qt](openfortivpn-webview-qt/)
 - [openfortivpn-webview-electron](openfortivpn-webview-electron/)

They should be equivalent, but `openfortivpn-webview-qt` may have
some issues with some SAML providers.

`openfortivpn-webview-electron` is readily available, see the
[instructions](https://github.com/gm-vm/openfortivpn-webview/tree/main/openfortivpn-webview-electron#install)
on how to install it.

## Usage

Obtain `SVPNCOOKIE` for host `vpn-gateway`:
```sh
openfortivpn-webview vpn-gateway
```

You can also specify an authentication realm (normally not required):
```sh
openfortivpn-webview vpn-gateway:1234 --realm=foo
```

By default the application builds the SAML URL using the given host,
port and realm. You can alternatively provide an already built URL:

```sh
openfortivpn-webview --url 'https://vpn-gateway:1234/remote/saml/start?realm=foo'
```

The application exits automatically as soon as it prints `SVPNCOOKIE` to
stdout. You can change this behavior passing `--keep-open`. The application
will in this case stay open and keep printing `SVPNCOOKIE` as its value
changes, thus generating a stream of text.

The application does not print `SVPNCOOKIE` until it finds a URL matching
the regular expression passed to `--url-regex`. If no regular expression
is specified, the application will look for URLs containing `/sslvpn/portal.html`.
Waiting for such URL allows to deal with concurrent VPN sessions when the
gateway is configured to allow a single active session.


The inner Chromium engine may print a lot of messages. You can disable them
to only see the messages of the application.

```sh
# If you use the Qt variant
QT_LOGGING_RULES="*=false;webview=true" QTWEBENGINE_CHROMIUM_FLAGS="--enable-logging --log-level=3" openfortivpn-webview vpn-gateway

# If you use the Electron variant
openfortivpn-webview --enable-logging --log-level=3 vpn-gateway
```

## Proxy servers

If you have to use an http proxy to access the vpn gateway or the SAML id
provider, you can pass the `--proxy-server` option to Chromium.

Note that when using the Electron variant, all command line options are
also passed along to Chromium.

```sh
# If you use the Qt variant
QTWEBENGINE_CHROMIUM_FLAGS="--proxy-server=proxy.example.com:8080" openfortivpn-webview vpn-gateway

# If you use the Electron variant
openfortivpn-webview vpn-gateway --proxy-server=proxy.example.com:8080
```

## Passing command line options when using `npm start`

If you use `npm start` to start the Electron variant, you need to separate
the command line options to the application from the command line options
to npm using `--`, like this:

```sh
npm start myvpnhost -- --proxy-server=proxy.example.com:8080
```
