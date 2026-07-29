'use strict'

const sc_utils = require('./utils.js');

exports.GetConfig = function () { return config; }

exports.GetConfigMXGNSS = function () {
    try {
        const conf = sc_utils.ReadConfig('mxgnss.conf.json', '/usr/local/etc/');
        conf.NaviValidityPeriod = 5000; // [#] in ms
        return conf;
    }
    catch { } // console.error(err); - it can be absent
}

const config = (() => {
    try {
        const conf_mx = sc_utils.ReadConfig('mx.conf.json', '/etc'); // get platform id in order to select appropriate settings
        const conf = sc_utils.ReadConfig('server.conf.json', ''); 
        for (const i in conf) { // add new config items in accordance with the platform id
            const platformId = i.split('_').pop();
            if (platformId == conf_mx.platform.id) {
                let paramId = i.slice(0, i.length - platformId.length - 1); // gets first part of the string; '- 1' is for '_'
                conf[paramId] = conf[i];
            }
        }
        return conf;
    }
    catch (err) {
        console.error(err); // it can be written only once when the device is started.
    }
})();
