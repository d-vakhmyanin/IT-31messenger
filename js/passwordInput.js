let client;
let password;
let login;
document.addEventListener("DOMContentLoaded", ready);
function ready() {
    client = new WebSocket("ws://25.98.71.144:8000");
    let search = new URLSearchParams(window.location.search);
    login = search.get('login');
    client.onmessage=function(message){
        let json = JSON.parse(message.data);
        if (json.error == 'none') {
            window.location.href = 'mainPage.html?id=' + json.userid;
        }
        else{
            alert("Incorrect password.");
        }
    }
}
function logIn() {
    password = document.getElementById("pass").value;
    if (password.length == 0) {
        alert('Please input password.')
        return;
    }
    let request = {
        id: 'connectUser',
        login: login,
        password: password
    }
    let json = JSON.stringify(request);

    client.send(json);
}
document.onkeypress = function(event){
    if(event.which == 13)
    logIn();
}