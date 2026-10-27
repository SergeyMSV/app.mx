'use strict'

const sc_utils = require('./utils.js');

exports.GetConfig = function () { return config; }

exports.GetConfigMXGNSS = function () {
    try {
        const conf = sc_utils.ReadConfig('mxgnss.conf.json', '/usr/local/etc', true);
        conf.NaviValidityPeriod = 5000; // [#] in ms
        return conf;
    }
    catch { } // console.error(err); - it can be absent
}

const config = (() => {
    try {
        const conf = sc_utils.ReadConfig('mx.conf.json', '/etc', true); // get platform id in order to select appropriate settings
        return conf;
    }
    catch (err) {
        console.error(err); // it can be written only once when the device is started.
    }
})();
