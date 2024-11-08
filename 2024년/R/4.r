install.packages("rpart")
install.packages("rpart.plot") # 오타 수정

library(rpart)
library(rpart.plot) # 오타 수정

data(iris)

tree_model <- rpart(Species ~ Sepal.Length + Sepal.Width + Petal.Length + Petal.Width, data = iris, method='class')


rpart.plot(tree_model, main="Tree for Iris")
