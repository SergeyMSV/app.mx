'use strict'

const np_fs = require('fs');

const PRODUICTION = process.env.NODE_ENV === 'production';

exports.PRODUICTION = PRODUICTION;
exports.PORT = process.env.PORT || 1337;

exports.GetHostname = function () { return hostname; }

exports.GetConfig = function () { return config; }

const config = (() => {
    try {
        const conf_mx = ReadConfigMX(); // get platform id in order to select appropriate settings

        let conf = np_fs.readFileSync('server.conf.json', 'utf-8');
        conf = JSON.parse(conf);

        for (const i in conf) { // add new config items in accordance with the platform id
            const platformId = i.split('_').pop();
            if (platformId == conf_mx.platform.id) {
                let paramId = i.slice(0, i.length - platformId.length - 1); // gets first part of the string; '- 1' is for '_'
                conf[paramId] = conf[i];
            }
        }

        if (!PRODUICTION)
            SetTestRootPaths(conf);

        return conf;
    }
    catch (err) {
        console.error(err); // it can be written only once when the device is started.
    }
})();

const hostname = (() => {
    try {
        return np_fs.readFileSync(config.hostname, 'utf-8');
    }
    catch (err) {
        console.error(err); // it can be written only once when the device is started.
    }
    return 'NONAME';
})();

function ReadConfig(a_filepath) {
    let conf = np_fs.readFileSync(a_filepath, 'utf-8');
    return JSON.parse(conf);
}

function ReadConfigMX() {
    const filename = 'mx.conf.json';
    return ReadConfig(!PRODUICTION ? filename : '/etc/' + filename);
}

function SetTestRootPaths(a_confServer) {
    for (const i in a_confServer) { // add prefixes to paths in accordance with the platform id
        let val = a_confServer[i];
        if (val != undefined && typeof (val) === 'string' && val.length > 0 && val[0] === '/') {
            a_confServer[i] = 'test_root_fs' + val;
        }
    }
}
