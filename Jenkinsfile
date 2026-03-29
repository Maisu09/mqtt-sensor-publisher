pipeline{
    agent any
    environment{
        COMPOSE_FILE = 'docker-compose.docker.yml'
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
    }
}