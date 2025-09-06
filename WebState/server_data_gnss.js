'use strict';

const sc_config = require('./config.js');

//const sc_status = require('./status.js'); - it's needed in order to set GNSS colour mark

const g_confMXGNSS = sc_config.GetConfigMXGNSS();
// const g_confMXNavi = sc_config.GetConfigMXNavi();

exports.GetPage = function () {
    return '<tr><td id="gnss_status" bgcolor="white"></td><td>GNSS:</td><td id="gnss"></td></tr>';
}

exports.GetPageData = function () {

    let files = fs.readdirSync(g_confMXGNSS.out.path);

    let data = {};
    if (g_confMXNavi !== undefined) {
        data.gnss_status = "yellow";
        data.gnss = "TEST_1";
    }
    else {
        data.gnss_status = "blue";
        data.gnss = "not available";
    }
    return data;
}

//class tThermalStatus {
//    constructor() {
//        this.m_Status = sc_status.CPU.Color.None;
//    }
//}
