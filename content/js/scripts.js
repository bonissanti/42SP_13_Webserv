// scripts.js

document.addEventListener('DOMContentLoaded', function() {
    loadFileList();
});

function loadFileList() {
    fetch('/files')
        .then(response => response.json())
        .then(files => {
            const fileList = document.getElementById('fileList');
            fileList.innerHTML = '';

            files.forEach(file => {
                const li = document.createElement('li');
                li.textContent = file.name;

                const deleteButton = document.createElement('button');
                deleteButton.textContent = 'Deletar';
                deleteButton.addEventListener('click', () => {
                    fetch(`/delete/${file.name}`, { method: 'DELETE' })
                        .then(() => loadFileList());
                });
                li.appendChild(deleteButton);
                fileList.appendChild(li);
            });
        });
}
