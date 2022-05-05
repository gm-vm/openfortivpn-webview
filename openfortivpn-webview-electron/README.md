Application to perform the SAML single sing-on and easily retrieve the
`SVPNCOOKIE` needed by `openfortivpn`.

The application will simply open the SAML page to let you sign in.
As soon as the `SVPNCOOKIE` is set, the application will print it to
stdout and exit.


## How to build

Install `npm` and run the following command:
```shell
npm install
```

This is enough to launch the application with:
```shell
node index.js
```

You can also build a package for your system using `electron-builder`.
For example, to build an `AppImage` and `tar.xz` for Linux, run:
```shell
./node_modules/.bin/electron-builder --linux AppImage tar.xz
```

This will generate a portable `tar.xz` in `dist/`.
See the documentation of `electron-builder` for more info.
