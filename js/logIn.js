let client;
let login;
document.addEventListener("DOMContentLoaded", ready);
function ready(){
     client = new WebSocket("ws://25.98.71.144:8000");
     client.onmessage=function (message) {
          let json = JSON.parse(message.data);
          if (json.error == 'none'&& json.result) {
              window.location.href = 'input.html?login=' + login;
          }
          if (json.error != 'none'){
               alert(json.error);
          }
          if (!json.result){
               alert("No such user :(");
          }
     }
};
function onward()
{
     login = document.getElementById('login').value;
     if (login.length==0) {
          alert('Please input login.')
          return;
     }
     let request = {
          id: 'isExist',
          login: login,
      };

      let json = JSON.stringify(request);
      
      client.send(json);
}
document.onkeypress = function(event){
     if(event.which == 13)
     onward();
}