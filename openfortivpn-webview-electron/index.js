const { app, BrowserWindow, session, Menu } = require('electron');
const yargs = require('yargs/yargs')
const { hideBin } = require('yargs/helpers')

const defaultUrlRegex = '/sslvpn/portal\\.html';
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
