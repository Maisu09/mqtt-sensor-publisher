pipeline{
    agent any
    environment{
        COMPOSE_FILE = 'docker-comnpose.docker.yaml'
    }
    stages{
        stage('Checkout'){
            steps {
                checkout scm
            }
        }
        stage('Verify Compose File') {
            steps{
                sh 'docker compose config'
            }
        }
        stage('Build Images') {
            steps{
                sh 'docker compose build'                
            }
        }
        stage('Deploy') {
            steps{
                sh 'docker compose up -d --remove-orphans || true'
            }
        }
    }

    post{
        success {
            echo 'Deployment successful!'
        }
        failure {
            echo 'Pipeline failed - check logs'
            sh 'docker compose down || true'
        }
    }
}