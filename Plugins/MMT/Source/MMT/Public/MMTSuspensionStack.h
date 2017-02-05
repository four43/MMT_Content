//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

//#include "Components/SceneComponent.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "MMTSuspensionStackStruct.h"
#include "MMTSuspensionStack.generated.h"

UCLASS(EditInlineNew)
class MMT_API UMMTSuspensionStack : public UObject
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMTSuspensionStack();

	//Delegate for async trace
	FTraceDelegate TraceDelegate;
	void AsyncTraceDone(const FTraceHandle& TraceHandle, FTraceDatum & TraceData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack Settings", meta = (ToolTip = "Suspension stack settings"))
		FSuspensionStackStruct SuspensionSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack Settings", meta = (ToolTip = "Suspension stack settings"))
		bool TryAsyncTraceMode = false;
	
	//Set reference to parent object. Suspension Stack is typically a sub-object of other components and can't refer to it's parent on it's own.
	void SetParentComponentReference(USceneComponent* Parent);

	/**
	*	Runs calculations of suspension stack, applies spring force to sprung component and calculates new position of the road-wheels
	*	@param DeltaTime				Delta time
	*	@return	WheelLocalPosition		Position of the road-wheel in local coordinate system of the actor, NOT a relative position to component
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void PhysicsUpdate(const float& DeltaTime, FVector& WheelLocalPosition);

	/**
	*	Initializes class logic, finds references and pre-calculates parameters. Expected to be called from BeginPlay only
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void Initialize();

	/**
	*	Toggle debug mode on/off
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void ToggleDebugMode();

	/**
	*	Get last calculated suspension force
	*	@return	Magnitude	Magnitude of the force as float
	*	@return	WorldSpace	Vector force in world space
	*	@return	LocalSpace	Vector force in local space
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
	void GetSuspensionForce(float &Magnitude, FVector& WorldSpace, FVector& LocalSpace);

	/**
	*	Get contact data at point where trace or sweep has succeeded. This information is necessary for friction component
	*	@return	bPointActive			Indicates if contact with surface was made
	*	@return	ForceAtPoint			Force at contact point in world space (suspension force pushing against surface)
	*	@return	PointLocation			Location of contact point in world space
	*	@return SurfaceNormal			Normal of the surface at contact point
	*	@return	SurfacePhysicalMaterial		Physical material of contact surface
	*	@return	SurfaceVelocity			Velocity of the body that contact was made with
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void GetContactData(bool& bPointActive, FVector& ForceAtPoint, FVector& PointLocation, FVector& SurfaceNormal, class UPhysicalMaterial*& SurfacePhysicalMaterial, FVector& SurfaceVelocity);

	/**
	*	Get last calculated suspension force
	*	@return		Wheel hub position in local space
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		FVector GetWheelHubPosition();

private:
	
	UPROPERTY()
		USceneComponent* ParentComponentRef;
	UPROPERTY()
		UMeshComponent* SprungMeshComponent;
	UPROPERTY()
		UMeshComponent* SweepShapeMeshComponent;
	UPROPERTY()
		FString ComponentName = FString("ComponentRefereneFailed");
	UPROPERTY()
		FString ComponentsParentName = FString("ParentRefereneFailed");
	UPROPERTY()
		float SpringMaxLenght;
	UPROPERTY()
		FVector SpringOffsetTop;
	UPROPERTY()
		FVector SpringOffsetBottom;
	UPROPERTY()
		FTransform ReferenceFrameTransform;
	UPROPERTY()
		FVector SpringDirectionLocal;
	UPROPERTY()
		bool bWarningMessageDisplayed = false;
	UPROPERTY()
		FVector WheelHubPositionLS;
	UPROPERTY()
		float PreviousSpringLenght;
	UPROPERTY()
		float SuspensionForceMagnitude;
	UPROPERTY()
		FVector SuspensionForceLS;
	UPROPERTY()
		FVector SuspensionForceWS;

	//Line trace specific
	FCollisionQueryParams LineTraceQueryParameters;
	FCollisionResponseParams LineTraceResponseParameters;
	UPROPERTY()
		FVector LineTraceOffsetTopLS;
	UPROPERTY()
		FVector LineTraceOffsetBottomLS;
	
	//Variables for recording collision of the wheel and passing data further to friction processing
	UPROPERTY()
		bool bContactPointActive;
	UPROPERTY()
		FVector ContactInducedVelocity;
	UPROPERTY()
		FVector ContactForceAtPoint;
	UPROPERTY()
		FVector ContactPointLocation;
	UPROPERTY()
		FVector ContactPointNormal;
	UPROPERTY()
		UPhysicalMaterial* ContactPhysicalMaterial;
	

	//Find reference to named components
	void GetNamesForComponentAndParent();

	void GetNamedComponentsReference();

	//Recalculate parameters to save performance
	void PreCalculateParameters();

	//Find new position of road-wheel according to current simulation mode
	void UpdateWheelHubPosition();

	//Find wheel hub position and contact information for RayCheck suspension mode
	void LineTraceForContact();
	void AsyncLineTraceForContact();

	//Calculate suspension force and apply to effected body
	void CalculateAndApplySuspensionForce(const float& DeltaTime);

	//Draw simple debug lines for suspension traces
	static void DrawDebugLineTrace(bool bBlockingHit, FVector Start, FVector End, FVector HitPoint, UWorld *WorldRef);

	//DoOnce for debug messages
	template<typename CallbackType> void DoOnce(const CallbackType& Callback) { static bool bDone = false; if (!bDone) { Callback(); bDone = true; } };
};
