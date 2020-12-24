let client;
let userid = 0;
let userName;
let userDialogs;

document.addEventListener("DOMContentLoaded", ready);
function ready() {
    client = new WebSocket("ws://25.98.71.144:8000");
    client.onmessage = function (message) {
        let json = JSON.parse(message.data);
        switch (json.id) {
            case 'reconnectUser':
                if (json.error == 'none') {
                    let request = {
                        id: 'getUserInfo',
                        userid: userid
                    };
                    let json = JSON.stringify(request);
                    client.send(json);
                    request = {
                        id: 'getChatsUser',
                        start: 0,
                        end: 100
                    };
                    json = JSON.stringify(request);
                    client.send(json);
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'getUserInfo':
                if (json.error == 'none') {
                    userName = json.name;
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'getChatsUser':
                if (json.error == 'none') {
                    userDialogs = json.arr;
                    showUserDialogs();
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'findUsers':
                if (json.error == 'none') {
                    addDialogToDialogsBlock(json.arr);
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'createChat':
                if (json.error == 'none') {
                    addUsersToChat(json.chatid);
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'addUserToChat':
                if (json.error == 'none') {
                    //alert('User was added to the chat');
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'getChatInfo':
                if (json.error == 'none') {
                    document.getElementById("hrbci").innerText += " (members: " +
                        json.members + ")";
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'createMsg':
                if (json.error == 'none') {
                    //date and time
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'getMsgsChat':
                if (json.error == 'none') {
                    addMessagesTop(json.arr);
                }
                else {
                    alert('Server Error.\n' + json.error);
                }
                break;
            case 'newMsg':
                addNewMessage(json.chatid, json.msg);
                break;
        }
    };
    client.onopen = function () {
        let search = new URLSearchParams(window.location.search);
        userid = parseInt(search.get('id'));
        let request = {
            id: 'reconnectUser',
            userid: userid
        };
        let json = JSON.stringify(request);
        client.send(json);
        alert('Welcome to Messenger 101!\nTo start, add a chat: click on the "Messenger" button and on "new group". Then select a user and enter a chat name.\nDone! Enjoy.');
    };
}

function dropDownMenu() {
    document.getElementsByClassName("header__left-bar-header__drop-down-menu")[0].classList.toggle("show");
}
function dialogSearch() {
    input = document.getElementsByClassName("left-bar__search");
    filter = input.item(0).value.toUpperCase();
    dialogs = document.getElementsByClassName("left-bar__dialogs-grid__dialog");
    dialogNames = document.getElementsByClassName("left-bar__dialogs-grid__dialog__name");
    for (i = 0; i < dialogNames.length; i++) {
        if (dialogNames[i].innerHTML.toUpperCase().indexOf(filter) > -1) {
            dialogs[i].style.display = "";
        } else {
            dialogs[i].style.display = "none";
        }
    }
}
function userSearch() {
    let input = document.getElementById("ngddbmcsb");
    let filter = input.value;
    let users = document.getElementById("ngddbmcmc").children;
    let userNames = document.getElementsByClassName("newGroupDialog__dialog-block__main-container_main-content__user_name");
    let request = {
        id: 'findUsers',
        text: filter,
        start: 0,
        end: 100
    };
    let json = JSON.stringify(request);
    client.send(json);
    for (i = 0; i < userNames.length; i++) {
        if (userNames[i].innerText.toUpperCase().indexOf(filter) > -1) {
            users[i].style.display = "";
        } else {
            users[i].style.display = "none";
        }
    }
}
function recreateMainContainer() {
    dialogs = document.getElementsByClassName("left-bar__dialogs-grid__dialog");
    for (i = 0; i < dialogs.length; i++) {
        dialogs[i].id = "lbdd" + i;
    }
    this.id = "active";
    document.getElementById("hrb").style.display = "inline-block";
    document.getElementById("hrbci").innerHTML = this.children[1].innerHTML;
    let request = {
        id: 'getChatInfo',
        chatid: parseInt(this.dataset.chatId),
        userid: parseInt(userid),
    };
    let json = JSON.stringify(request);
    client.send(json);
    //убираем старые сообщения
    oldMessages = document.getElementsByClassName("main-container__message-history__message")
    while (oldMessages.length > 1) {
        if (oldMessages[1].id != 'hidden-message') oldMessages[1].remove();
        else oldMessages[0].remove();
    }
    //и старый неотправленный текст
    $("#mcpgfta")[0].emojioneArea.setText('');

    //Обработка нижней панели
    document.getElementById("mcpgci").style.backgroundImage = 'url(' + this.children[0].src + ')';
    document.getElementById("mcpgui").style.backgroundImage = 'url(img/user.png)';

    //Отображение скрытых элементов
    document.getElementById("def").style.display = "none";
    document.getElementById("mcmh").style.display = "flex";
    document.getElementById("mcmhtw").style.display = "block";
    document.getElementById("mcp").style.display = "block";
    document.getElementsByClassName("main-container__message-history__message")[0].style.backgroundColor = 'white';
    document.getElementsByClassName("emojionearea-editor")[0].addEventListener('keydown', function (event) {
        let input = $("#mcpgfta")[0].emojioneArea.getText();
        if (event.key == "Enter" && !event.shiftKey) {
            event.preventDefault();
            if (input.length == 0) {
                $("#mcpgfta")[0].emojioneArea.setText("");
                return;
            }
            sendMessage();
        }
    });
    let block = document.getElementById("mcmh");
    block.scrollTop = block.scrollHeight;
}
function check() {
    if (this.id != "active")
        this.id = "hovered";
}
function uncheck() {
    if (this.id != "active")
        this.id = "lbdd" + Array.from(document.getElementById('lbd').children).indexOf(this);
}
function sendMessage() {
    let input = $("#mcpgfta")[0].emojioneArea.getText();
    let inputArray = input.split('\n');
    input = '';
    for (let i = 0; i < inputArray.length; i++) {
        inputArray[i] = inputArray[i].trim();
        if (inputArray[i].length > 0)
            input += '\n' + inputArray[i];
    }
    if (input.length == 0) {
        $("#mcpgfta")[0].emojioneArea.setText("");
        return;
    }
    input = input.slice(1);
    let thisChatId = document.getElementById("active").dataset.chatId;
    let request = {
        id: 'createMsg',
        chatid: parseInt(thisChatId),
        answerid: -1,
        type: 0,
        text: input
    };
    let json = JSON.stringify(request);
    client.send(json);
    input = input.replaceAll("\n", "<br>");
    //создаём сообщение в чате
    let date = new Date();
    let minutes = date.getMinutes();
    let hours = date.getHours();
    let seconds = date.getSeconds();
    if (minutes < 10) minutes = '0' + String(minutes);
    if (hours < 10) hours = '0' + String(hours);
    if (seconds < 10) seconds = '0' + String(seconds);
    let newMessage = document.createElement("div");
    newMessage.className = "main-container__message-history__message";
    newMessage.innerHTML = document.getElementsByClassName("main-container__message-history__message")[0].innerHTML;
    newMessage.children[0].style.backgroundImage = 'url(img/user.png)';
    newMessage.children[1].children[0].textContent = userName;
    newMessage.children[1].children[1].innerHTML = input;
    newMessage.children[2].innerHTML = hours + ":" + minutes + ":" + seconds;
    newMessage.style.backgroundColor = 'white';
    newMessage.onclick = function () {
        if (newMessage.style.backgroundColor == 'white') newMessage.style.backgroundColor = 'rgb(243, 246, 250)';
        else newMessage.style.backgroundColor = 'white';
        let activeMessages = new Array();
        for (let i = 0; i < document.getElementById('mcmh').children.length; i++) {
            if (document.getElementById('mcmh').children[i].style.backgroundColor == 'rgb(243, 246, 250)') {
                activeMessages.push(document.getElementById('mcmh').children[i]);
            }
        }
        if (activeMessages.length == 0) {
            document.getElementById("mcp").style.display = 'block';
            document.getElementById("mcgsmp").style.display = 'none';
            document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
            document.getElementById("mcmhtw").style.borderBlockEnd = 'none';

        }
        else {
            document.getElementById("mcp").style.display = 'none';
            document.getElementById("mcgsmp").style.display = 'flex';
            document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
            document.getElementById("mcgsmpfbf").textContent += '(' + activeMessages.length + ')';
            document.getElementById("mcmhtw").style.borderBlockEnd = '1px solid black';
        }
    };
    document.getElementById("mcmh").appendChild(newMessage);

    //изменяем последнее сообщение в диалоге на левой панели 
    let curDialog = document.getElementById("active");
    //let imgWithoutURL=newMessage.children[0].backgroundImage.substring(4,newMessage.children[0].backgroundImage.length-1);
    //curDialog.children[0].src=imgWithoutURL;
    curDialog.children[2].textContent = hours + ":" + minutes;
    curDialog.children[3].textContent = "You: " + newMessage.children[1].children[1].innerText;

    let block = document.getElementById("mcmh");
    block.scrollTop = block.scrollHeight;

    $("#mcpgfta")[0].emojioneArea.setText("");
    $("#mcpgfta")[0].emojioneArea.setFocus();
}
function recreatePanel() {
    if (this.style.backgroundColor == 'white') this.style.backgroundColor = 'rgb(243, 246, 250)';
    else this.style.backgroundColor = 'white';
    let activeMessages = new Array();
    for (let i = 0; i < document.getElementById('mcmh').children.length; i++) {
        if (document.getElementById('mcmh').children[i].style.backgroundColor == 'rgb(243, 246, 250)') {
            activeMessages.push(document.getElementById('mcmh').children[i]);
        }
    }
    if (activeMessages.length == 0) {
        document.getElementById("mcp").style.display = 'block';
        document.getElementById("mcgsmp").style.display = 'none';
        document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
        document.getElementById("mcmhtw").style.borderBlockEnd = 'none';

    }
    else {
        document.getElementById("mcp").style.display = 'none';
        document.getElementById("mcgsmp").style.display = 'flex';
        document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
        document.getElementById("mcgsmpfbf").textContent += '(' + activeMessages.length + ')';
        document.getElementById("mcmhtw").style.borderBlockEnd = '1px solid black';
    }
}
function canselSelection() {
    for (let i = 0; i < document.getElementById('mcmh').children.length; i++) {
        document.getElementById('mcmh').children[i].style.backgroundColor = 'white';
    }
    document.getElementById("mcp").style.display = 'block';
    document.getElementById("mcgsmp").style.display = 'none';
    document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
    document.getElementById("mcmhtw").style.borderBlockEnd = 'none';
}
function displayNewGroup() {
    document.getElementById("ngd").style.display = 'flow-root';
    document.getElementById("ngddb").style.display = 'block';
    let users = document.getElementById("ngddbmcmc").children;
    for (let i = 0; i < users.length; i++)
        users[i].className = "newGroupDialog__dialog-block__main-container_main-content__user";
    document.getElementById("ngddbfn").innerText = "Next";
}
function closeNewDialog() {
    document.getElementById("ngd").style.display = 'none';
    document.getElementById("ngddbmcsb").value = "";
    let dialogsBlock = document.getElementById("ngddbmcmc");
    while (dialogsBlock.children.length != 1) dialogsBlock.children[1].remove();

}
function addDialogToDialogsBlock(users) {
    let dialogsBlock = document.getElementById("ngddbmcmc");
    while (dialogsBlock.children.length != 1) dialogsBlock.children[1].remove();
    for (let i = 0; i < users.length; i++) {
        let user = document.createElement("div");
        user.className = "newGroupDialog__dialog-block__main-container_main-content__user";
        user.innerHTML = dialogsBlock.children[0].innerHTML;
        user.id = users[i].userid;
        //user.style.display='grid';
        //user.children[0].style.backgroundImage = 'url(img/mouse.png)';
        user.children[1].innerText = users[i].name;
        user.children[2].innerText = users[i].state;
        user.onmousemove = function hoverUser() {
            if (this.className == "newGroupDialog__dialog-block__main-container_main-content__user")
                this.className = "hoveredUsers";
        };
        user.onmouseout = function unhoverUser() {
            if (this.className == "hoveredUsers")
                this.className = "newGroupDialog__dialog-block__main-container_main-content__user";
        };
        user.onclick = function checkUser() {
            if (this.className == "activeUsers") {
                this.className = "hoveredUsers"
            }
            else if (this.className == "hoveredUsers") {
                this.className = "activeUsers";
            }
            let count = document.getElementsByClassName("activeUsers").length;
            if (count > 0)
                document.getElementById("ngddbfn").innerText = "Next(" + count + ")";
            else document.getElementById("ngddbfn").innerText = "Next";
        };
        dialogsBlock.appendChild(user);
    }
}
function checkUser() {
    if (this.className == "activeUsers") {
        this.className = "hoveredUsers"
    }
    else if (this.className == "hoveredUsers") {
        this.className = "activeUsers";
    }
    let count = document.getElementsByClassName("activeUsers").length;
    if (count > 0)
        document.getElementById("ngddbfn").innerText = "Next(" + count + ")";
    else document.getElementById("ngddbfn").innerText = "Next";
}
function hoverUser() {
    if (this.className == "newGroupDialog__dialog-block__main-container_main-content__user")
        this.className = "hoveredUsers";
}
function unhoverUser() {
    if (this.className == "hoveredUsers")
        this.className = "newGroupDialog__dialog-block__main-container_main-content__user";
}
function nextClick() {
    let btnText = document.getElementById("ngddbfn").innerText;
    if (btnText == 'Next') return;
    let prevDialog = document.getElementById("ngddb");
    prevDialog.style.display = "none";
    let dialog = document.getElementById("ngdcd");
    dialog.style.display = "block";
}
function cancelCreatingDialogClick() {
    let dialog = document.getElementById("ngdcd");
    dialog.style.display = "none";
    document.getElementById("ngdcdmcti").value = "";
    document.getElementById("ngd").style.display = "none";
    closeNewDialog();
}
function createNewGroup() {
    let chatName = document.getElementById("ngdcdmcti").value;
    if (chatName == "") return;
    let request = {
        id: 'createChat',
        name: chatName,
        info: "",
        type: 0
    };
    let json = JSON.stringify(request);
    client.send(json);
}
function addUsersToChat(chatId) {
    let users = document.getElementsByClassName("activeUsers");
    while (users.length != 0) {
        let request = {
            id: 'addUserToChat',
            chatid: chatId,
            userid: parseInt(users[0].id) - 1,
            type: 0
        };
        let json = JSON.stringify(request);
        client.send(json);
        users[0].remove();
    }
    addNewDialog(chatId);
    cancelCreatingDialogClick();
}
function addNewDialog(chatId) {
    let chatName = document.getElementById("ngdcdmcti").value;
    let dialogsLeftBar = document.getElementById("lbd");
    let dialog = document.createElement("div");
    dialog.className = "left-bar__dialogs-grid__dialog";
    dialog.innerHTML = dialogsLeftBar.children[0].innerHTML;
    dialog.id = "lbdd" + dialogsLeftBar.children.length;
    dialog.dataset.chatId = chatId;
    dialog.children[1].innerText = chatName;
    let date = new Date();
    let minutes = date.getMinutes();
    let hours = date.getHours();
    dialog.children[2].innerText = hours + ":" + minutes;
    dialog.children[3].innerText = "Created";
    dialog.onclick = function () {
        dialogs = document.getElementsByClassName("left-bar__dialogs-grid__dialog");
        for (i = 0; i < dialogs.length; i++) {
            dialogs[i].id = "lbdd" + i;
        }
        dialog.children[4].style.display='none';
        this.id = "active";
        document.getElementById("hrb").style.display = "inline-block";
        document.getElementById("hrbci").innerHTML = this.children[1].innerHTML;
        //запрос количества участников чата
        let request = {
            id: 'getChatInfo',
            chatid: parseInt(this.dataset.chatId),
            userid: parseInt(userid),
        };
        let json = JSON.stringify(request);
        client.send(json);
        //запрос сообщений чата
        request = {
            id: 'getMsgsChat',
            chatid: parseInt(this.dataset.chatId),
            start: 0,
            end: 100
        };
        json = JSON.stringify(request);
        client.send(json);

        //убираем старые сообщения
        oldMessages = document.getElementsByClassName("main-container__message-history__message")
        while (oldMessages.length > 1) {
            if (oldMessages[1].id != 'hidden-message') oldMessages[1].remove();
            else oldMessages[0].remove();
        }
        //и старый неотправленный текст
        $("#mcpgfta")[0].emojioneArea.setText('');

        //Обработка нижней панели
        document.getElementById("mcpgci").style.backgroundImage = 'url(' + this.children[0].src + ')';
        document.getElementById("mcpgui").style.backgroundImage = 'url(img/user.png)';

        //Отображение скрытых элементов
        document.getElementById("def").style.display = "none";
        document.getElementById("mcmh").style.display = "flex";
        document.getElementById("mcmhtw").style.display = "block";
        document.getElementById("mcp").style.display = "block";
        document.getElementsByClassName("main-container__message-history__message")[0].style.backgroundColor = 'white';
        document.getElementsByClassName("emojionearea-editor")[0].addEventListener('keydown', function (event) {
            let input = $("#mcpgfta")[0].emojioneArea.getText();
            if (event.key == "Enter" && !event.shiftKey) {
                event.preventDefault();
                if (input.length == 0) {
                    $("#mcpgfta")[0].emojioneArea.setText("");
                    return;
                }
                sendMessage();
            }
        });
        let block = document.getElementById("mcmh");
        block.scrollTop = block.scrollHeight;
    };
    dialog.onmouseover = function () {
        if (this.id != "active")
            this.id = "hovered";
    };
    dialog.onmouseout = function () {
        if (this.id != "active")
            this.id = "lbdd" + Array.from(document.getElementById('lbd').children).indexOf(this);
    };
    dialogsLeftBar.appendChild(dialog);
}
function showUserDialogs() {
    let dialogsLeftBar = document.getElementById("lbd");
    for (let i = 0; i < userDialogs.length; i++) {
        let chatName = userDialogs[i].name;
        let chatId = userDialogs[i].chatid;
        let lastMessage = userDialogs[i].msg;
        let dialog = document.createElement("div");
        dialog.className = "left-bar__dialogs-grid__dialog";
        dialog.innerHTML = dialogsLeftBar.children[0].innerHTML;
        dialog.id = "lbdd" + dialogsLeftBar.children.length;
        dialog.dataset.chatId = chatId;
        dialog.children[1].innerText = chatName;

        dialog.children[2].innerText = lastMessage.time.substring(0, 5);
        if (lastMessage.userid == userid) dialog.children[3].innerText = "You:" + lastMessage.text;
        else dialog.children[3].innerText = lastMessage.name + ": " + lastMessage.text;
        dialog.onclick = function () {
            dialogs = document.getElementsByClassName("left-bar__dialogs-grid__dialog");
            for (i = 0; i < dialogs.length; i++) {
                dialogs[i].id = "lbdd" + i;
            }
            dialog.children[4].style.display='none';
            this.id = "active";
            document.getElementById("hrb").style.display = "inline-block";
            document.getElementById("hrbci").innerHTML = this.children[1].innerHTML;
            //запрос количества участников чата
            let request = {
                id: 'getChatInfo',
                chatid: parseInt(this.dataset.chatId),
                userid: parseInt(userid),
            };
            let json = JSON.stringify(request);
            client.send(json);
            //запрос сообщений чата
            request = {
                id: 'getMsgsChat',
                chatid: parseInt(this.dataset.chatId),
                start: 0,
                end: 100
            };
            json = JSON.stringify(request);
            client.send(json);
            //убираем старые сообщения
            oldMessages = document.getElementsByClassName("main-container__message-history__message")
            while (oldMessages.length > 1) {
                if (oldMessages[1].id != 'hidden-message') oldMessages[1].remove();
                else oldMessages[0].remove();
            }
            //и старый неотправленный текст
            $("#mcpgfta")[0].emojioneArea.setText('');

            //Обработка нижней панели
            document.getElementById("mcpgci").style.backgroundImage = 'url(' + this.children[0].src + ')';
            document.getElementById("mcpgui").style.backgroundImage = 'url(img/user.png)';

            //Отображение скрытых элементов
            document.getElementById("def").style.display = "none";
            document.getElementById("mcmh").style.display = "flex";
            document.getElementById("mcmhtw").style.display = "block";
            document.getElementById("mcp").style.display = "block";
            document.getElementsByClassName("main-container__message-history__message")[0].style.backgroundColor = 'white';
            document.getElementsByClassName("emojionearea-editor")[0].addEventListener('keydown', function (event) {
                let input = $("#mcpgfta")[0].emojioneArea.getText();
                if (event.key == "Enter" && !event.shiftKey) {
                    event.preventDefault();
                    if (input.length == 0) {
                        $("#mcpgfta")[0].emojioneArea.setText("");
                        return;
                    }
                    sendMessage();
                }
            });
            let block = document.getElementById("mcmh");
            block.scrollTop = block.scrollHeight;
        };
        dialog.onmouseover = function () {
            if (this.id != "active")
                this.id = "hovered";
        };
        dialog.onmouseout = function () {
            if (this.id != "active")
                this.id = "lbdd" + Array.from(document.getElementById('lbd').children).indexOf(this);
        };
        dialogsLeftBar.appendChild(dialog);
    }
}
function addMessagesTop(messages) {

    let firstMessage = document.getElementById("hidden-message");
    //создаём сообщение в чате
    for (let i = 0; i < messages.length; i++) {
        let newMessage = document.createElement("div");
        newMessage.className = "main-container__message-history__message";
        newMessage.innerHTML = firstMessage.innerHTML;
        if (messages[i].userid == parseInt(userid)) newMessage.children[0].style.backgroundImage = 'url(img/user.png)';
        else newMessage.children[0].style.backgroundImage = 'url(img/mouse.png)';
        newMessage.children[1].children[0].textContent = messages[i].name;
        newMessage.children[1].children[1].innerHTML = messages[i].text;
        newMessage.children[2].innerHTML = messages[i].time;
        newMessage.style.backgroundColor = 'white';
        newMessage.onclick = function () {
            if (newMessage.style.backgroundColor == 'white') newMessage.style.backgroundColor = 'rgb(243, 246, 250)';
            else newMessage.style.backgroundColor = 'white';
            let activeMessages = new Array();
            for (let i = 0; i < document.getElementById('mcmh').children.length; i++) {
                if (document.getElementById('mcmh').children[i].style.backgroundColor == 'rgb(243, 246, 250)') {
                    activeMessages.push(document.getElementById('mcmh').children[i]);
                }
            }
            if (activeMessages.length == 0) {
                document.getElementById("mcp").style.display = 'block';
                document.getElementById("mcgsmp").style.display = 'none';
                document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
                document.getElementById("mcmhtw").style.borderBlockEnd = 'none';

            }
            else {
                document.getElementById("mcp").style.display = 'none';
                document.getElementById("mcgsmp").style.display = 'flex';
                document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
                document.getElementById("mcgsmpfbf").textContent += '(' + activeMessages.length + ')';
                document.getElementById("mcmhtw").style.borderBlockEnd = '1px solid black';
            }
        };

        document.getElementById("mcmh").insertBefore(newMessage, firstMessage);
    }
    let block = document.getElementById("mcmh");
    block.scrollTop = block.scrollHeight;
}
function addNewMessage(chatId, message) {
    if (message.userid==userid) return;
    let curChat = document.getElementById("active");
    if (curChat != null) {
        let thisChatId = parseInt(curChat.dataset.chatId);
        if (thisChatId == chatId) {
            let firstMessage = document.getElementById("hidden-message");
            let newMessage = document.createElement("div");
            newMessage.className = "main-container__message-history__message";
            newMessage.innerHTML = firstMessage.innerHTML;
            newMessage.children[0].style.backgroundImage = 'url(img/mouse.png)';
            newMessage.children[1].children[0].textContent = message.name;
            newMessage.children[1].children[1].innerHTML = message.text;
            newMessage.children[2].innerHTML = message.time;
            newMessage.style.backgroundColor = 'white';
            newMessage.onclick = function () {
                if (newMessage.style.backgroundColor == 'white') newMessage.style.backgroundColor = 'rgb(243, 246, 250)';
                else newMessage.style.backgroundColor = 'white';
                let activeMessages = new Array();
                for (let i = 0; i < document.getElementById('mcmh').children.length; i++) {
                    if (document.getElementById('mcmh').children[i].style.backgroundColor == 'rgb(243, 246, 250)') {
                        activeMessages.push(document.getElementById('mcmh').children[i]);
                    }
                }
                if (activeMessages.length == 0) {
                    document.getElementById("mcp").style.display = 'block';
                    document.getElementById("mcgsmp").style.display = 'none';
                    document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
                    document.getElementById("mcmhtw").style.borderBlockEnd = 'none';

                }
                else {
                    document.getElementById("mcp").style.display = 'none';
                    document.getElementById("mcgsmp").style.display = 'flex';
                    document.getElementById("mcgsmpfbf").textContent = 'FORWARD';
                    document.getElementById("mcgsmpfbf").textContent += '(' + activeMessages.length + ')';
                    document.getElementById("mcmhtw").style.borderBlockEnd = '1px solid black';
                }
            };
            document.getElementById("mcmh").appendChild(newMessage);
        }
    }
    let block = document.getElementById("mcmh");
    block.scrollTop = block.scrollHeight;

    let chatIdWithCov="\""+chatId+"\"";
    let dialog = document.querySelectorAll('[data-chat-id=' + chatIdWithCov + ']')[0];
    dialog.children[2].innerText = message.time.substring(0, 5);
    dialog.children[3].innerText = message.name+":"+ message.text;
    if (!(curChat != null && thisChatId == chatId)) dialog.children[4].style.display='block';
}