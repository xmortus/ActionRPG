#include "Skills/Implementations/SkillRegeneration.h"
#include "Components/Combat/StatusEffectComponent.h"

bool USkillRegeneration::CanActivate(AActor* Target) const
{
	AActor* Recipient = Target ? Target : OwnerActor.Get();
	return EffectDefinition && Recipient && Recipient->FindComponentByClass<UStatusEffectComponent>()
		&& Super::CanActivate(Target);
}

USkillBase* USkillRegeneration::Activate(AActor* Target)
{
	AActor* Recipient = Target ? Target : OwnerActor.Get();
	UStatusEffectComponent* Effects = Recipient ? Recipient->FindComponentByClass<UStatusEffectComponent>() : nullptr;
	if (!Effects || !EffectDefinition || !Super::Activate(Target)) return nullptr;
	return Effects->ApplyEffect(EffectDefinition, OwnerActor.Get()) ? this : nullptr;
}
