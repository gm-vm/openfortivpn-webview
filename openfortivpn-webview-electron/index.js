const { app, BrowserWindow, session, Menu } = require('electron');
const yargs = require('yargs/yargs')
const { hideBin } = require('yargs/helpers')
const { Console } = require('console');

const errorConsole = new Console(process.stderr);

const defaultUrlRegex = '/sslvpn/portal(/|\\.html)';
const cookieName = 'SVPNCOOKIE';

const parser = yargs(hideBin(process.argv))
  .command('[host:port]', 'Connect to the given host')
  .option('realm', {
      describe: 'The authentication realm.',
      type: "string",
  })
  .option('url', {
      describe: 'The already built SAML URL. This takes precedence over [host:port].',
      type: "string",
  })
  .option('url-regex', {
      describe: "A regex to detect the URL that needs to be visited before printing SVPNCOOKIE.",
      type: "string",
      default: defaultUrlRegex,
  })
  .option('keep-open', {
      describe: 'Do not close the browser automatically.',
  })
  .option('proxy-server', {
      describe: 'HTTP Proxy in the format hostname:port.',
      type: "string",
  })
  .option('extra-ca-certs', {
      describe: 'Path to a file with extra certificates. The file should consist of one or more trusted certificates in PEM format.',
      type: "string",
  })
  .option('trusted-cert', {
      describe: 'The fingerprint of a certificate to always trust, even if invalid. The details of invalid certificates, fingerprint included, will be dumped in the console.',
      type: "string",
  })
  .help();

const argv = parser.parse();

if (argv._.length == 0 && !argv.url) {
  parser.showHelp()
  process.exit(1);
}

const urlBuilder = () => {
  if (argv.url) {
    return argv.url;
  } else {
    const realm = argv.realm ? `?realm=${argv.realm}` : '';
    return `https://${argv._[0]}/remote/saml/start${realm}`;
  }
};

const urlRegex = RegExp(argv['url-regex'] ? argv['url-regex'] : defaultUrlRegex);

app.on('certificate-error', (event, webContents, url, error, certificate, callback) => {
  if (argv['trusted-cert'] === certificate.fingerprint) { 
    event.preventDefault();
    callback(true);
  } else {
    errorConsole.error('Found an invalid certificate:');
    errorConsole.dir(certificate, { depth: null });
    errorConsole.error();
    errorConsole.error('If you know that this certificate can be trusted, relaunch the application passing the following argument to ignore the error:');
    errorConsole.error(`--trusted-cert='${certificate.fingerprint}'`);
    process.exit(1);
  }
})

if (argv['extra-ca-certs']) {

  const pemHeader = '-----BEGIN CERTIFICATE-----';
  const pemFooter = '-----END CERTIFICATE-----';

  // The binary representation allows to easily compare certificates that use different line-endings.
  let pemToBytes = pem => {
    const start = pem.indexOf(pemHeader) + pemHeader.length;
    const end = pem.indexOf(pemFooter);
    return atob(pem.substring(start, end));
  }

  try {
    const fileContent = require('fs').readFileSync(argv['extra-ca-certs'], 'utf8');
    const certExtractionRegex = new RegExp(`${pemHeader}[^-]+${pemFooter}`, 'g');
    const extraCaCerts = new Set(fileContent.match(certExtractionRegex).map(pem => pemToBytes(pem)));

    let issuerPem = certificate => {
      // issuerCert of self-signed certificates is undefined.
      return certificate.issuerCert ? certificate.issuerCert.data : certificate.data;
    }

    app.on('certificate-error', (event, webContents, url, error, certificate, callback) => {
      if (
        error === 'net::ERR_CERT_AUTHORITY_INVALID' &&
        extraCaCerts.has(pemToBytes(issuerPem(certificate)))
      ) {
        event.preventDefault();
        callback(true);
      } else {
        callback(false);
      }
    });
  } catch (e) {
    errorConsole.error(e.message);
    process.exit(1);
  }
}

app.whenReady().then(() => {
  const window = new BrowserWindow({ width: 800, height: 600 });

  var shouldPrintCookie = false;
  var svpncookie = null;

  const tryPrintCookie = () => {
    if (shouldPrintCookie && svpncookie != null) {
      process.stdout.write(`${cookieName}=${svpncookie}\n`);
      if (!argv['keep-open']) {
        process.exit(0);
      }
    }
  }

  window.webContents.on('did-start-navigation', (e, url) => {
    process.stderr.write(url + '\n');
    if (urlRegex.test(url)) {
      shouldPrintCookie = true;
      tryPrintCookie();
    }
  });

  session.defaultSession.cookies.on('changed', (e, cookie) => {
    if (cookie.name == cookieName) {
      svpncookie = cookie.value;
      tryPrintCookie();
    }
  });

  window.on('close', function() {
    process.exit(argv['keep-open'] ? 0 : 1);
  });

  const menu = Menu.buildFromTemplate([{
    label: "File",
    submenu: [
      {
        role: "reload",
      },
      {
        type: "separator",
      },
      {
        label: "Clear data",
        click: () => { session.defaultSession.clearStorageData() },
      },
      {
        type: "separator",
      },
      {
        label: 'Quit App',
        accelerator: 'CmdOrCtrl+Q',
        click: () => { app.quit() },
      },
    ]
  },{
    label: "Edit",
    submenu: [
        { label: "Undo", accelerator: "CmdOrCtrl+Z", role: "undo" },
        { label: "Redo", accelerator: "Shift+CmdOrCtrl+Z", role: "redo" },
        { type: "separator" },
        { label: "Cut", accelerator: "CmdOrCtrl+X", role: "cut" },
        { label: "Copy", accelerator: "CmdOrCtrl+C", role: "copy" },
        { label: "Paste", accelerator: "CmdOrCtrl+V", role: "paste" },
        { label: "Select All", accelerator: "CmdOrCtrl+A", role: "selectAll" },
    ]
  }]);
  Menu.setApplicationMenu(menu);

  window.loadURL(urlBuilder());
});
