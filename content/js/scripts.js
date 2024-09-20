document.addEventListener('DOMContentLoaded', function() {

    const uploadForm = document.getElementById('uploadForm');
    uploadForm.addEventListener('submit', function(event) {
        event.preventDefault(); // Evita o recarregamento da página padrão

        const formData = new FormData(uploadForm);
        
        fetch('../uploads', {
            method: 'POST',
            body: formData
        })
        .then(response => response.text()) // Supondo que seu server retorna uma mensagem simples
        .then(result => {
            alert(result);  // Exibe mensagem de sucesso
        })
        .catch(error => {
            console.error('Erro no upload:', error);
        });
    });
});


