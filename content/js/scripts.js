document.addEventListener('DOMContentLoaded', function() {
    loadFileList();

    const uploadForm = document.getElementById('uploadForm');
    uploadForm.addEventListener('submit', function(event) {
        event.preventDefault(); // Evita o recarregamento da página padrão

        const formData = new FormData(uploadForm);
        
        fetch('../upload', {
            method: 'POST',
            body: formData
        })
        .then(response => response.text()) // Supondo que seu server retorna uma mensagem simples
        .then(result => {
            alert(result);  // Exibe mensagem de sucesso
            loadFileList();  // Atualiza a lista de arquivos após o upload
        })
        .catch(error => {
            console.error('Erro no upload:', error);
        });
    });
});

function loadFileList() {
    fetch('/files')  // A rota no servidor que lista os arquivos
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
        })
        .catch(error => {
            console.error('Erro ao carregar lista de arquivos:', error);
        });
}

