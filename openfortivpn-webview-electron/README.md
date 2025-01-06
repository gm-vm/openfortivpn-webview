Application to perform the SAML single sign-on and easily retrieve the
`SVPNCOOKIE` needed by `openfortivpn`.

The application will simply open the SAML page to let you sign in.
As soon as the `SVPNCOOKIE` is set, the application will print it to
stdout and exit.


## Install

### Linux
You can download the application as `AppImage` or `tar.xz` from the
[Releases page](https://github.com/gm-vm/openfortivpn-webview/releases).

The `AppImage` is an executable that can be launched as is.
The `tar.xz` contains the `openfortivpn-webview` executable.

### macOS
If you have HomeBrew installed, you can install `openfortivpn-webview` with:
```sh
brew install --no-quarantine gm-vm/repo/openfortivpn-webview
```
You'll then be able to simply launch `openfortivpn-webview` from the terminal.

You can otherwise download the `.dmg` from the [Releases page](https://github.com/gm-vm/openfortivpn-webview/releases)
and install it. Since the application is not signed, macOS will not allow
to run it unless you give it explicit permission from the system settings.
You can achieve the same result by simply running the following command:
```sh
xattr -rd com.apple.quarantine /Applications/openfortivpn-webview.app
```

The path to the `openfortivpn-webview` binary will be:
```sh
/Applications/openfortivpn-webview.app/Contents/MacOS/openfortivpn-webview
```


## How to build

Install `npm` and run the following command:
```shell
npm install
```

This is enough to launch the application with:
```shell
npm start
```

You can also build a package for your system using `electron-builder`.
For example, to build an `AppImage` and `tar.xz` for Linux, run:
```shell
./node_modules/.bin/electron-builder --linux AppImage tar.xz
```

This will generate a portable `tar.xz` in `dist/`.
See the documentation of `electron-builder` for more info.
