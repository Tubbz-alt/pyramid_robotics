#include "pyramid_control/actuator_controller.h"
#define PRINT_MAT(X) cout << #X << ":\n" << X << endl << endl

namespace pyramid_control
{

ActuatorController::ActuatorController()
{
    lpp_.AddVariableSet(std::make_shared<ExVariables>());
    lpp_.AddCostSet(std::make_shared<ExCost>());
    lpp_.PrintCurrent();

    ipopt_.SetOption("linear_solver", "mumps");
    ipopt_.SetOption("jacobian_approximation", "exact");
    ipopt_.SetOption("print_level", 1);
}

ActuatorController::~ActuatorController(){ }

void ActuatorController::InitializeParameters()
{
    calculateAllocationMatrix(system_parameters_.rotor_configuration_, &allocation_matrix_);
}

void ActuatorController::wrenchDistribution(const Eigen::VectorXd& wrench,
                                            const Eigen::MatrixXd& jacobian,
                                            const Eigen::Matrix3d& rotMatrix,
                                            const Eigen::Matrix3d& toOmega)
{
    Eigen::MatrixXd MatA = -jacobian.transpose();
    Eigen::MatrixXd MatB = calcRotorMatrix(rotMatrix) * allocation_matrix_;
    Eigen::MatrixXd MatAB(MatA.rows(), MatA.cols()+MatB.cols());
    MatAB << MatA, MatB;

    Eigen::Matrix<double, 6, 6> MatS = Eigen::MatrixXd::Zero(6, 6);
    MatS.block<3, 3>(0, 0) = Eigen::Matrix3d::Identity();
    MatS.block<3, 3>(3, 3) = toOmega;

    MatAB = MatS.transpose() * MatAB;

    Eigen::MatrixXd distributionMatrix = MatAB.transpose() * (MatAB * MatAB.transpose()).inverse();

    distributedWrench_ = distributionMatrix * wrench;

    Eigen::FullPivLU<Eigen::MatrixXd> lu(MatAB);
    lu.setThreshold(1e-5);
    kernel_ = lu.kernel();
    rank_ = lu.rank();
}

void ActuatorController::optimize()
{
    if (feasibility() == false)
    {
        if(rank_ == 6)
        {
            lpp_.AddConstraintSet(std::make_shared<ExConstraint>("constraint1",
                                                                 kernel_, distributedWrench_));
            ipopt_.Solve(lpp_);
            Eigen::VectorXd x = lpp_.GetOptVariables()->GetValues();

            distributedWrench_ += kernel_ * x;

            tension_ = distributedWrench_.block<8, 1>(0, 0);
            tension_ = tension_.cwiseMax(Eigen::VectorXd::Zero(tension_.rows()));
            motor_speed_ = distributedWrench_.block<4, 1>(8, 0);
            motor_speed_ = motor_speed_.cwiseMax(Eigen::VectorXd::Zero(motor_speed_.rows()));
            motor_speed_ = motor_speed_.cwiseSqrt();

            lpp_.Clear();
        }
    }

}

} //namespace pyramid_control
