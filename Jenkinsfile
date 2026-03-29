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
    }
}