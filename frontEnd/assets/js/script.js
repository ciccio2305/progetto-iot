let a =  document.querySelectorAll('path');
let green = '#00ff00';
let red = '#ff0000';
let yellow = '#ffff00';

function changeColor(color) {
    a[0].style.stroke = color;
    a[1].style.fill = color;
}






let list = document.querySelector('.list');
function createListItem(){
    

    let li = document.createElement('div');
    li.classList.add('listItem');

    let name = document.createElement('div');
    name.classList.add('name');
    name.innerHTML = 'Nome Cognome';

    let status = document.createElement('div');
    status.classList.add('status');
    
    let statusPlaceholder = document.createElement('div');
    statusPlaceholder.classList.add('statusPlaceholder');
    statusPlaceholder.innerHTML = 'Status: ';

    let statusIcon = document.createElement('div');
    statusIcon.classList.add('statusIcon');
    statusIcon.innerHTML = `<svg width="100%" height="100%" viewBox="0 0 22 20" fill="none" xmlns="http://www.w3.org/2000/svg">
    <path d="M17 9.9999H16.1986C15.3689 9.9999 14.9541 9.9999 14.6102 10.1946C14.2664 10.3893 14.0529 10.745 13.6261 11.4564L13.5952 11.5079C13.1976 12.1706 12.9987 12.502 12.7095 12.4965C12.4202 12.4911 12.2339 12.1525 11.8615 11.4753L10.1742 8.4075C9.8269 7.77606 9.6533 7.46034 9.3759 7.44537C9.0986 7.43039 8.892 7.72558 8.47875 8.3159L8.19573 8.7203C7.75681 9.3473 7.53734 9.6608 7.21173 9.8303C6.88612 9.9999 6.50342 9.9999 5.73803 9.9999H5" stroke="#ffffff" stroke-width="1.5" stroke-linecap="round"/>
    <path d="M11 3.5006L10.4596 4.0207C10.601 4.1676 10.7961 4.2506 11 4.2506C11.2039 4.2506 11.399 4.1676 11.5404 4.0207L11 3.5006ZM1.65666 11.3964C1.87558 11.748 2.33811 11.8556 2.68974 11.6367C3.04137 11.4178 3.14895 10.9552 2.93003 10.6036L1.65666 11.3964ZM5.52969 13.7718C5.23645 13.4793 4.76158 13.4798 4.46903 13.7731C4.17649 14.0663 4.17706 14.5412 4.47031 14.8337L5.52969 13.7718ZM1.75 7.13707C1.75 4.33419 3.00722 2.59507 4.57921 1.99711C6.15546 1.39753 8.35129 1.8302 10.4596 4.0207L11.5404 2.9805C9.1489 0.495831 6.3447 -0.27931 4.04591 0.59512C1.74286 1.47116 0.25 3.88785 0.25 7.13707H1.75ZM14.5026 17.4999C15.9949 16.3234 17.7837 14.7461 19.2061 12.9838C20.6126 11.2412 21.75 9.2089 21.75 7.13703H20.25C20.25 8.688 19.3777 10.3829 18.0389 12.0417C16.716 13.6807 15.0239 15.1788 13.574 16.3219L14.5026 17.4999ZM21.75 7.13703C21.75 3.88784 20.2571 1.47115 17.9541 0.59511C15.6553 -0.2793 12.8511 0.495831 10.4596 2.9805L11.5404 4.0207C13.6487 1.8302 15.8445 1.39753 17.4208 1.99711C18.9928 2.59506 20.25 4.33418 20.25 7.13703H21.75ZM7.49742 17.4998C8.77172 18.5044 9.6501 19.2359 11 19.2359V17.7359C10.2693 17.7359 9.8157 17.4174 8.42605 16.3219L7.49742 17.4998ZM13.574 16.3219C12.1843 17.4174 11.7307 17.7359 11 17.7359V19.2359C12.3499 19.2359 13.2283 18.5044 14.5026 17.4999L13.574 16.3219ZM2.93003 10.6036C2.18403 9.4054 1.75 8.2312 1.75 7.13707H0.25C0.25 8.617 0.83054 10.0695 1.65666 11.3964L2.93003 10.6036ZM8.42605 16.3219C7.50908 15.599 6.49093 14.7307 5.52969 13.7718L4.47031 14.8337C5.48347 15.8445 6.54819 16.7515 7.49742 17.4998L8.42605 16.3219Z" fill="#ffffff"/>
    </svg>`;

    status.appendChild(statusPlaceholder);
    status.appendChild(statusIcon);

    li.appendChild(name);
    li.appendChild(status);

    list.appendChild(li);
}
for(let i = 0; i < 15; i++)
createListItem();

let dashboard = document.querySelector('.right');

function createDashboardItem(){
    let card = document.createElement('div');
    card.classList.add('card');

    dashboard.appendChild(card);
}