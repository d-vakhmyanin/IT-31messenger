let client;
let login;
document.addEventListener("DOMContentLoaded", ready);
function ready(){
    client = new WebSocket("ws://25.98.71.144:8000");

    client.onmessage = function (message) {
        let json = JSON.parse(message.data);
        if (json.error == 'none') {
            window.location.href = 'mainPage.html?id=' + json.userid;
        }
        else {
            alert(json.error);
        }
    }
}


function lostFocusLogin() {
    let input = document.getElementsByClassName('fixed_item')[0];
    let text = document.getElementsByClassName('text-small-information')[0];

    if (!input.value.match(/^[a-zA-Z0-9]+$/)) {
        input.style.border = '2px solid red';
        text.textContent = 'логин должен содержать только буквы и цифры';
        return false;
    }
    else{
        input.style.border = 'none';
        text.textContent = '';
        return true;
    }
}

function lostFocusName(){
    let input = document.getElementsByClassName('fixed_item')[1];
    let text = document.getElementsByClassName('text-small-information')[1];

    if(!input.value.match(/^([A-Za-z]+$)/)){
        input.style.border = '2px solid red';
        text.textContent = 'Имя должно содержать только буквы';
        return false;
    }
    else{
        input.style.border = 'none';
        text.textContent = '';
        return true;
    }
}

function lostFocusSurname(){
    let input = document.getElementsByClassName('fixed_item')[2];
    let text = document.getElementsByClassName('text-small-information')[2];

    if(!input.value.match(/^([A-Za-z]+$)/)){
        input.style.border = '2px solid red';
        text.textContent = 'Фамилия должна содержать только буквы';
        return false;
    }
    else{
        input.style.border = 'none';
        text.textContent = '';
        return true;
    }
}

function lostFocusPassword(){
    let input = document.getElementsByClassName('fixed_item')[3];
    let text = document.getElementsByClassName('text-small-information')[3];

    if(!input.value.match(/^[a-zA-Z0-9]+$/)){
        input.style.border = '2px solid red';
        text.textContent = 'Пароль должен содержать только буквы и цифры.';
        return false;
    }
    else{
        input.style.border = 'none';
        text.textContent = '';
        return true;
    }
}

function lostFucusPasswordConfirmation() {
    let password = document.getElementsByClassName('fixed_item')[3];
    let passwordTrue = document.getElementsByClassName('fixed_item')[4];
    let text = document.getElementsByClassName('text-small-information')[4];

    if (password.value != passwordTrue.value) {
        passwordTrue.style.border = '2px solid red';  
        text.textContent = 'Пароли не совпадают';
        return false;
    }
    else{
        passwordTrue.style.border = 'none';
        text.textContent = '';
        return true;
    }

}

function check() {
    let input = document.getElementsByClassName('fixed_item');
    
    if(!(lostFocusLogin() && (lostFocusName() || lostFocusSurname()) && lostFocusPassword() && lostFucusPasswordConfirmation()))
        return;

    let user = {
        id: 'createUser',
        name: input[1].value + ' ' + input[2].value,
        login: input[0].value,
        password: input[3].value,
        info: ''
    };

    let juser = JSON.stringify(user);
    
        client.send(juser);
    
}
//при закрытии страницы обрывать соединение
    window.unload = ()=>{
        client.close();
    }

//отправка формы по Enter
document.onkeypress = function(event){
    if(event.keyCode == 13)
        check();
}






