var SerialPort = require('serialport'); // include the serialport library
var portName = 'COM6'; // get the port name from the command line
var myPort = new SerialPort(portName, 9600);// open the port
myPort.on('open', ()=>{
  console.log('Port Settings:\n', myPort.settings);
  var serial_handler = (serial_resp)=>{}

  myPort.on('readable', function () {
    serial_handler(myPort.read().toString('utf8'))
  })


  const express = require('express')
  const path = require('path')
  const app = express()
  app.listen(9090, () => {
    console.log(`Example app listening at http://localhost:9090`)
  })

  app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname + '/index.html'));
  })

  let cur_player = ''
  let password = ''
  const cryptoRandomString = require('crypto-random-string');
  app.get('/start', (req, res) => {
    if(cur_player!=''){
      res.status(409).send("Sorry please wait until " + cur_player + " is done")
    }
    else{
      myPort.write("START\n")
      cur_player = req.query.username
      password = cryptoRandomString({length: 100, type:'url-safe'})
      res.send(password)
    }
  })

  app.get('/status', function (req, res) {
    if(cur_player=='')
      res.send("Ready!")
    else
      res.send(cur_player + " is playing")
  });

  app.get('/stop', (req, res) => {
    if(password==''){
      res.status(409).send("No game in progress to stop")
    }
    else if(password!=req.query.password){
      res.status(409).send("You cant stop this game")
    }
    else{
      cur_player = ''
      password = ''
      myPort.write("STOP\n")
      serial_handler = (serial_resp)=>{
        res.send(serial_resp);
      }
    }

  })

});
